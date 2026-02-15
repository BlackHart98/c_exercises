# Retry: fixed handling for no-discharge-event case and edge cases. Re-run brute-force enumeration.
import itertools, math, pandas as pd

days = list(range(1,7))  # days 1..6
# Parameters (same as before)
Kmax = 2  # up to 2 trips per daughter
L_max = 1
U_max = 1
n_grace = 1.0  # grace days before demurrage applies
c_unused = 2.0  # penalty per unit unused mother capacity
# Daughter data
daughters = {
    'A': {'cap': 120, 'T': 3, 'delta': 1, 't_arr_first': 1, 'c_dem': 100, 'feas_load_days': [1,4], 'feas_disc_days': [2,5]},
    'B': {'cap': 150, 'T': 3, 'delta': 2, 't_arr_first': 1, 'c_dem': 120, 'feas_load_days': [1,2,4], 'feas_disc_days': [2,3,5]},
}
# Mother
mothers = {'M1': {'cap': 300, 'min_parcel': 200, 'planned_dep': 5, 'c_dem': 200}}

# Helper functions
def gen_choices_for_daughter(name):
    feas = daughters[name]['feas_load_days']
    options = [0] + feas
    all_tuples = list(itertools.product(options, repeat=Kmax))
    valid = []
    for tup in all_tuples:
        ok = True
        last_start = None
        for t in tup:
            if t == 0:
                continue
            if t not in feas:
                ok = False; break
            if last_start is not None:
                if t < last_start + daughters[name]['T']:
                    ok = False; break
            last_start = t
        if ok:
            valid.append(tup)
    return valid

choices_A = gen_choices_for_daughter('A')
choices_B = gen_choices_for_daughter('B')

def evaluate_schedule(choiceA, choiceB):
    load_events = []
    for name, choice in [('A', choiceA), ('B', choiceB)]:
        for k, t in enumerate(choice, start=1):
            if t != 0:
                load_events.append({'name': name, 'k': k, 't_load': t})
    # Loading berth constraint
    load_count = {d:0 for d in days}
    for ev in load_events:
        load_count[ev['t_load']] += 1
    if any(v > L_max for v in load_count.values()):
        return None
    # discharge arrivals
    disc_arrivals = []
    for ev in load_events:
        name = ev['name']; t_load = ev['t_load']
        t_arr_disc = t_load + daughters[name]['delta']
        if t_arr_disc not in days:
            return None
        disc_arrivals.append({'name': name, 'k': ev['k'], 't_arr_disc': t_arr_disc, 'cap': daughters[name]['cap']})
    # arrivals_by_day
    arrivals_by_day = {d: [] for d in days}
    for ev in disc_arrivals:
        arrivals_by_day[ev['t_arr_disc']].append(ev)
    pending = []
    discharge_schedule = {}
    # Simulate day-by-day
    for t in days:
        for ev in arrivals_by_day[t]:
            pending.append(ev.copy())
        if not pending:
            continue
        # feasible today
        feasible_today = [p for p in pending if t in daughters[p['name']]['feas_disc_days']]
        if not feasible_today:
            continue
        feasible_today.sort(key=lambda x: daughters[x['name']]['c_dem'], reverse=True)
        to_serve = feasible_today[:U_max]
        for s in to_serve:
            discharge_schedule[(s['name'], s['k'])] = t
            # remove one matching pending
            for idx, p in enumerate(pending):
                if p['name']==s['name'] and p['k']==s['k']:
                    pending.pop(idx); break
    if pending:
        return None
    # If no discharge events, treat as feasible: compute unused penalty only
    if not disc_arrivals:
        total_assigned = 0
        Uload = 0
        unused = mothers['M1']['cap']
        total_unused_penalty = c_unused * unused
        total_daughter_dem = 0.0
        mother_dem_cost = 0.0
        total_cost = total_unused_penalty
        details = {'load_events': load_events, 'discharge_arrivals': disc_arrivals,
                   'discharge_schedule': discharge_schedule, 'daughter_dem_cost': total_daughter_dem,
                   'unused': unused, 'unused_penalty': total_unused_penalty, 'mother_dem_cost': mother_dem_cost,
                   'total_cost': total_cost, 'completion_day': None}
        return details
    # compute waiting and billable
    W_load = {}; W_disch = {}; billable = {}
    total_daughter_dem = 0.0
    for name in ['A','B']:
        choice = choiceA if name=='A' else choiceB
        for k_idx, t_load in enumerate(choice, start=1):
            if t_load==0:
                continue
            if k_idx==1:
                t_arr_load = daughters[name]['t_arr_first']
            else:
                prev = choice[k_idx-2]
                if prev==0:
                    # if previous didn't occur, assume arrival as first arrival + T*(k-1)
                    t_arr_load = daughters[name]['t_arr_first'] + (k_idx-1)*daughters[name]['T']
                else:
                    t_arr_load = prev + daughters[name]['T']
            wload = max(0, t_load - t_arr_load)
            W_load[(name,k_idx)] = wload
            t_arr_disc = t_load + daughters[name]['delta']
            tsched_disc = discharge_schedule.get((name,k_idx), None)
            if tsched_disc is None:
                return None
            wdisch = max(0, tsched_disc - t_arr_disc)
            W_disch[(name,k_idx)] = wdisch
            gload = max(0, wload - n_grace)
            gdisch = max(0, wdisch - n_grace)
            billable[(name,k_idx)] = (gload, gdisch)
            total_daughter_dem += daughters[name]['c_dem'] * (gload + gdisch)
    total_assigned = sum(ev['cap'] for ev in disc_arrivals)
    Uload = total_assigned
    unused = max(0, mothers['M1']['cap'] - Uload)
    total_unused_penalty = c_unused * unused
    # mother demurrage
    if discharge_schedule:
        completion_day = max(discharge_schedule.values())
        mother_dem_days = max(0, completion_day - mothers['M1']['planned_dep'])
        mother_dem_cost = mother_dem_days * mothers['M1']['c_dem']
    else:
        completion_day = None; mother_dem_cost = 0.0
    total_cost = total_daughter_dem + total_unused_penalty + mother_dem_cost
    details = {'load_events': load_events, 'discharge_arrivals': disc_arrivals,
               'discharge_schedule': discharge_schedule, 'W_load': W_load, 'W_disch': W_disch,
               'billable': billable, 'daughter_dem_cost': total_daughter_dem, 'unused': unused,
               'unused_penalty': total_unused_penalty, 'mother_dem_cost': mother_dem_cost,
               'total_cost': total_cost, 'completion_day': completion_day}
    return details

# enumerate and show results
results = []
for choiceA in choices_A:
    for choiceB in choices_B:
        det = evaluate_schedule(choiceA, choiceB)
        if det is not None:
            results.append({'choiceA': choiceA, 'choiceB': choiceB, 'total_cost': det['total_cost'], 'details': det})
results.sort(key=lambda x: x['total_cost'])
df_rows = []
for r in results:
    df_rows.append({
        'choiceA': r['choiceA'], 'choiceB': r['choiceB'], 'total_cost': r['total_cost'],
        'daughter_dem_cost': r['details']['daughter_dem_cost'], 'unused': r['details']['unused'],
        'unused_penalty': r['details']['unused_penalty'], 'mother_dem_cost': r['details']['mother_dem_cost'],
        'completion_day': r['details']['completion_day']
    })
df = pd.DataFrame(df_rows).sort_values('total_cost').reset_index(drop=True)
# import caas_jupyter_tools as tools; tools.display_dataframe_to_user("Feasible schedules (sorted by cost) v2", df)

if results:
    best = results[0]
    print("Best schedule total cost:", best['total_cost'])
    print("choiceA (trip1,trip2):", best['choiceA'])
    print("choiceB (trip1,trip2):", best['choiceB'])
    print("Discharge schedule (daughter,trip)->day:", best['details']['discharge_schedule'])
    print("Waiting before load:", best['details'].get('W_load', {}))
    print("Waiting before discharge:", best['details'].get('W_disch', {}))
    print("Billable waiting (gload,gdisch):", best['details'].get('billable', {}))
    print("Daughter demurrage cost:", best['details']['daughter_dem_cost'])
    print("Unused capacity:", best['details']['unused'], "penalty:", best['details']['unused_penalty'])
    print("Mother demurrage cost:", best['details']['mother_dem_cost'])
else:
    print("No feasible schedules found.")
