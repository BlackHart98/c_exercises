# Fixing brute-force simulator: enforce mother min_parcel constraint properly,
# and tighten some checks. Re-run the small scenario and report top schedules.
import itertools, copy, pandas as pd
from collections import defaultdict

days = list(range(1,9))
Kmax = 2

daughters = {
    'A': {
        'cap': 100, 'Delta': 1, 'T': 2, 'c_dem': 100, 'n_load': 1.0, 'n_disch': 1.0,
        'loc': 'L1',
        'FeasLoad': {t:1 for t in days},
        'FeasDiscDays': set(range(2,9))
    },
    'B': {
        'cap': 120, 'Delta': 1, 'T': 3, 'c_dem': 120, 'n_load': 1.0, 'n_disch': 1.0,
        'loc': 'L1',
        'FeasLoad': {t:1 for t in days},
        'FeasDiscDays': set(range(2,9))
    }
}

mothers = {
    'M1': {'cap': 220, 'min_parcel': 200, 'planned_dep': 6, 'c_dem': 200, 'c_unused': 2.0, 'port': 'P1'}
}

ports = {'P1': {'U_max': 0, 'P_max': 2}}
L_max = {'L1': 1}
I0 = {'L1': 1000}

def gen_valid_start_tuples(name):
    feasible_days = [d for d in days if daughters[name]['FeasLoad'].get(d,0)==1]
    options = [0] + feasible_days
    valid = []
    for tup in itertools.product(options, repeat=Kmax):
        ok = True
        prev = None
        for t in tup:
            if t==0:
                continue
            if prev is not None:
                if t < prev + daughters[name]['Delta'] + daughters[name]['T']:
                    ok = False; break
            prev = t
        if ok:
            valid.append(tup)
    return valid

choices = {name: gen_valid_start_tuples(name) for name in daughters}

def simulate(choiceA, choiceB):
    # build events
    y_events = []
    for name, choice in [('A',choiceA),('B',choiceB)]:
        for k,t in enumerate(choice, start=1):
            if t!=0:
                if daughters[name]['FeasLoad'].get(t,0)!=1:
                    return None
                y_events.append({'name':name,'k':k,'t_load':t})
    # berth check
    load_count = defaultdict(int)
    for ev in y_events:
        load_count[(daughters[ev['name']]['loc'], ev['t_load'])] += 1
    for (loc,t),cnt in load_count.items():
        if cnt > L_max[loc]: return None
    # arrivals
    arrivals = []
    for ev in y_events:
        arr = ev['t_load'] + daughters[ev['name']]['Delta']
        if arr not in days: return None
        arrivals.append({'name':ev['name'],'k':ev['k'],'arr':arr,'cap':daughters[ev['name']]['cap']})
    # Quick check: if total assigned volume (all arrivals) > mother cap -> infeasible
    total_arrival_volume = sum(a['cap'] for a in arrivals)
    if total_arrival_volume > mothers['M1']['cap']:
        return None
    # Now simulate discharges with queue and U_max
    arrivals_by_day = defaultdict(list)
    for a in arrivals:
        arrivals_by_day[a['arr']].append(a)
    pending = []
    discharge_schedule = {}
    for t in days:
        for ev in arrivals_by_day.get(t,[]): pending.append(copy.copy(ev))
        feasible_today = [p for p in pending if t in daughters[p['name']]['FeasDiscDays']]
        if not feasible_today: continue
        feasible_today.sort(key=lambda x: daughters[x['name']]['c_dem'], reverse=True)
        to_serve = feasible_today[:ports['P1']['U_max']]
        for s in to_serve:
            discharge_schedule[(s['name'], s['k'])] = t
            for idx,p in enumerate(pending):
                if p['name']==s['name'] and p['k']==s['k']:
                    pending.pop(idx); break
    if pending:
        return None
    # Now compute mother utilization properly: only count those actually discharged
    discharged = list(discharge_schedule.keys())
    total_assigned = sum(daughters[name]['cap'] for (name,k) in discharged)
    # enforce min_parcel: if any cargo assigned (total_assigned>0) then must be >= min_parcel
    if total_assigned>0 and total_assigned < mothers['M1']['min_parcel']:
        # treat as infeasible (could alternatively penalize heavily)
        return None
    # compute waits and costs
    daughter_dem_cost = 0.0
    wait_load = {}; wait_disch={}; billable={}
    for name, choice in [('A',choiceA),('B',choiceB)]:
        for k,tstart in enumerate(choice, start=1):
            if tstart==0: continue
            S = tstart; A = S + daughters[name]['Delta']
            Tdisc = discharge_schedule.get((name,k), None)
            if Tdisc is None: return None
            if k==1:
                ready_load = 1
            else:
                prev = choice[k-2]
                if prev==0:
                    ready_load = 1 + (k-1)*daughters[name]['T']
                else:
                    prev_disc = discharge_schedule.get((name,k-1), None)
                    if prev_disc is None: return None
                    ready_load = prev_disc + daughters[name]['T']
            Wload = max(0, S - ready_load)
            Wdisch = max(0, Tdisc - A)
            wait_load[(name,k)] = Wload; wait_disch[(name,k)] = Wdisch
            gload = max(0, Wload - daughters[name]['n_load']); gdisch = max(0, Wdisch - daughters[name]['n_disch'])
            billable[(name,k)] = (gload, gdisch)
            daughter_dem_cost += daughters[name]['c_dem'] * (gload + gdisch)
    unused = max(0, mothers['M1']['cap'] - total_assigned)
    unused_penalty = mothers['M1']['c_unused'] * unused
    if discharge_schedule:
        completion_day = max(discharge_schedule.values())
        mother_dem_days = max(0, completion_day - mothers['M1']['planned_dep'])
        mother_dem_cost = mother_dem_days * mothers['M1']['c_dem']
    else:
        completion_day = None; mother_dem_cost = 0.0
    total_cost = daughter_dem_cost + unused_penalty + mother_dem_cost
    return {
        'y_events': y_events, 'arrivals': arrivals, 'discharge_schedule': discharge_schedule,
        'wait_load': wait_load, 'wait_disch': wait_disch, 'billable': billable,
        'daughter_dem_cost': daughter_dem_cost, 'unused': unused, 'unused_penalty': unused_penalty,
        'mother_dem_cost': mother_dem_cost, 'total_cost': total_cost, 'completion_day': completion_day
    }

results = []
for cA in choices['A']:
    for cB in choices['B']:
        det = simulate(cA,cB)
        if det is not None:
            results.append({'choiceA': cA, 'choiceB': cB, 'total_cost': det['total_cost'], 'details': det})

results.sort(key=lambda x: x['total_cost'])
rows = []
for r in results[:20]:
    rows.append({'choiceA': r['choiceA'], 'choiceB': r['choiceB'], 'total_cost': r['total_cost'],
                 'daughter_dem_cost': r['details']['daughter_dem_cost'], 'unused': r['details']['unused'],
                 'unused_penalty': r['details']['unused_penalty'], 'mother_dem_cost': r['details']['mother_dem_cost'],
                 'completion_day': r['details']['completion_day']})
df = pd.DataFrame(rows)
# import caas_jupyter_tools as tools; tools.display_dataframe_to_user("Top brute-force schedules - fixed min_parcel", df)

if results:
    best = results[0]
    print("BEST total cost:", best['total_cost'])
    print("choiceA:", best['choiceA'], "choiceB:", best['choiceB'])
    print("y_events:", best['details']['y_events'])
    print("arrivals:", best['details']['arrivals'])
    print("discharge schedule:", best['details']['discharge_schedule'])
    print("wait_load:", best['details']['wait_load'])
    print("wait_disch:", best['details']['wait_disch'])
    print("billable:", best['details']['billable'])
    print("daughter dem cost:", best['details']['daughter_dem_cost'])
    print("unused:", best['details']['unused'], "unused penalty:", best['details']['unused_penalty'])
    print("mother dem cost:", best['details']['mother_dem_cost'])
else:
    print("No feasible schedules found.")
