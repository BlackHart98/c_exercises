# Small worked example: vessel scheduling with daughter demurrage (brute-force evaluator)
# This script brute-forces feasible schedules for a small instance (3 daughters, 2 mothers, 7 days).
# It evaluates schedules by assigning each daughter a load start day (from a small feasible set)
# and assigning each daughter to a mother. It then simulates discharge scheduling under U_max capacity,
# allowing discharge delays and computing daughter waiting and demurrage, as well as mother demurrage.
# Finally, it finds the least-cost feasible schedule under the assumptions described in the conversation.

import itertools
import pandas as pd

# --- Problem data ---
days = list(range(1, 8))  # days 1..7

# Daughters: id -> params
daughters = {
    'A': {
        'capacity': 100,
        'roundtrip_days': 2,   # deterministic roundtrip: load start -> next load ready after 2 days
        'load_duration': 1,    # days required to load (rounded)
        't_arrival_load': 1,   # earliest day vessel is available to start loading
        'demurrage_per_day': 100,
        'feasible_load_days': [1, 2, 3, 4, 5, 6, 7]  # due to tide feasibility, can only start loading on these days 
    },
    # 'B': {
    #     'capacity': 80,
    #     'roundtrip_days': 3,
    #     'load_duration': 1,
    #     't_arrival_load': 1,
    #     'demurrage_per_day': 120,
    #     'feasible_load_days': [2, 4]
    # },
    # 'C': {
    #     'capacity': 120,
    #     'roundtrip_days': 2,
    #     'load_duration': 2,
    #     't_arrival_load': 1,
    #     'demurrage_per_day': 110,
    #     'feasible_load_days': [1, 2]
    # },
}

# Mothers: id -> params
mothers = {
    'M1': {
        'capacity': 200,
        'min_parcel': 50,
        'planned_departure': 5,  # planned departure day
        'demurrage_per_day': 200
    },
    'M2': {
        'capacity': 150,
        'min_parcel': 60,
        'planned_departure': 5,
        'demurrage_per_day': 180
    }
}

L_max = 1  # loading berths per day
U_max = 1  # discharge berths per day (shared across mothers)
# We assume loading storage always stocked -> no A_t constraint

# --- Helper functions ---
def evaluate_schedule(load_choices, mother_assignments):
    """
    load_choices: dict daughter -> chosen load day
    mother_assignments: dict daughter -> mother id
    Returns (feasible(bool), details(dict), total_cost)
    """
    # 1. Check loading berth capacity per day
    load_count = {t: 0 for t in days}
    for i, t in load_choices.items():
        load_count[t] += 1
    if any(cnt > L_max for cnt in load_count.values()):
        return False, None, float('inf')
    
    # 2. Compute discharge arrival days (arrival_day = load_day + load_duration + transit (assume transit 0))
    discharge_arrival = {}
    for i in daughters:
        tload = load_choices[i]
        dload = daughters[i]['load_duration']
        arrival = tload + dload  # arrival day for discharge (integer)
        # if arrival beyond horizon, treat as infeasible for this simple example
        if arrival > days[-1]:
            return False, None, float('inf')
        discharge_arrival[i] = arrival
    
    # 3. Check mothers capacity totals (each daughter will take full capacity)
    mother_totals = {m: 0 for m in mothers}
    for i in daughters:
        mom = mother_assignments[i]
        mother_totals[mom] += daughters[i]['capacity']
    # If any mother capacity exceeded -> infeasible
    for m, tot in mother_totals.items():
        if tot > mothers[m]['capacity']:
            return False, None, float('inf')
    # If a mother receives any cargo, ensure min_parcel satisfied (else infeasible)
    for m, tot in mother_totals.items():
        if tot > 0 and tot < mothers[m]['min_parcel']:
            return False, None, float('inf')
    
    # 4. Schedule discharges day-by-day with U_max capacity and minimize daughter demurrage:
    # Use greedy choice on each day: serve arrivals with highest demurrage_per_day first.
    # Pending queue carries to next day.
    pending = []  # list of tuples (daughter_id, arrival_day)
    # initialize pending by arrival_day
    arrivals_by_day = {t: [] for t in days}
    for i, a in discharge_arrival.items():
        arrivals_by_day[a].append(i)
    
    discharge_schedule = {}  # daughter -> actual discharge day
    # Also track U_max per day usage
    for t in days:
        # add new arrivals
        for i in arrivals_by_day[t]:
            pending.append((i, t))
        if not pending:
            continue
        # sort pending by demurrage_per_day descending (serve highest cost first)
        pending.sort(key=lambda item: daughters[item[0]]['demurrage_per_day'], reverse=True)
        # serve up to U_max
        to_serve = pending[:U_max]
        remaining = pending[U_max:]
        for (i, arr) in to_serve:
            discharge_schedule[i] = t
        pending = remaining
    
    # After horizon, if pending still exist, treat as infeasible for simplicity (or could continue horizon)
    if pending:
        return False, None, float('inf')
    
    # 5. Compute waiting times and demurrage costs
    W_load = {}
    W_disch = {}
    daughter_demurrage_cost = 0.0
    for i in daughters:
        tarr_load = daughters[i]['t_arrival_load']
        tsched_load = load_choices[i]
        wload = max(0, tsched_load - tarr_load)
        W_load[i] = wload
        
        tarr_disch = discharge_arrival[i]  # expected arrival for discharge
        tsched_disch = discharge_schedule[i]
        wdisch = max(0, tsched_disch - tarr_disch)
        W_disch[i] = wdisch
        
        daughter_demurrage_cost += daughters[i]['demurrage_per_day'] * (wload + wdisch)
    
    # 6. Compute mother demurrage: completion day is max discharge day for that mother
    mother_completion = {m: None for m in mothers}
    for i, mom in mother_assignments.items():
        dday = discharge_schedule[i]
        if mother_completion[mom] is None or dday > mother_completion[mom]:
            mother_completion[mom] = dday
    mother_demurrage_cost = 0.0
    mother_demurrage_days = {}
    for m, comp in mother_completion.items():
        if comp is None:
            mother_demurrage_days[m] = 0
            continue
        dep = mothers[m]['planned_departure']
        dem_days = max(0, comp - dep)
        mother_demurrage_days[m] = dem_days
        mother_demurrage_cost += mothers[m]['demurrage_per_day'] * dem_days
    
    total_cost = daughter_demurrage_cost + mother_demurrage_cost
    
    details = {
        'load_choices': dict(load_choices),
        'mother_assignments': dict(mother_assignments),
        'discharge_arrival': dict(discharge_arrival),
        'discharge_schedule': dict(discharge_schedule),
        'W_load': W_load,
        'W_disch': W_disch,
        'daughter_demurrage_cost': daughter_demurrage_cost,
        'mother_demurrage_cost': mother_demurrage_cost,
        'mother_completion': mother_completion,
        'mother_demurrage_days': mother_demurrage_days,
        'total_cost': total_cost
    }
    
    return True, details, total_cost


# --- Enumerate schedules ---
# For each daughter, choices are its feasible load days; each daughter has 2 choices -> 2^3 combos.
# For mother assignment, each daughter chooses either M1 or M2 -> 2^3 combos.
feasible_schedules = []
for load_choice_tuple in itertools.product(*[daughters[i]['feasible_load_days'] for i in daughters]):
    load_choice = dict(zip(list(daughters.keys()), load_choice_tuple))
    # check virus L_max quickly before mother assignments
    # but evaluate per mother assignment below
    for mother_assign_tuple in itertools.product(['M1','M2'], repeat=len(daughters)):
        mother_assign = dict(zip(list(daughters.keys()), mother_assign_tuple))
        feasible, details, cost = evaluate_schedule(load_choice, mother_assign)
        if feasible:
            rec = {
                'load_choice': load_choice,
                'mother_assign': mother_assign,
                'total_cost': cost,
                'daughter_cost': details['daughter_demurrage_cost'],
                'mother_cost': details['mother_demurrage_cost'],
                'details': details
            }
            feasible_schedules.append(rec)

# Sort feasible schedules by cost
feasible_schedules.sort(key=lambda r: r['total_cost'])

# Present top 6 schedules
top6 = feasible_schedules[:6]
rows = []
for r in top6:
    rows.append({
        'load_choice': r['load_choice'],
        'mother_assign': r['mother_assign'],
        'total_cost': r['total_cost'],
        'daughter_cost': r['daughter_cost'],
        'mother_cost': r['mother_cost']
    })

df_top = pd.DataFrame(rows)

# import caas_jupyter_tools as tools; tools.display_dataframe_to_user("Top schedules (by total cost)", df_top)

# Also print the best schedule details
if feasible_schedules:
    best = feasible_schedules[0]
    print("Best schedule total cost:", best['total_cost'])
    print("Load choices (daughter -> day):", best['load_choice'])
    print("Mother assignments (daughter -> mother):", best['mother_assign'])
    print("Discharge arrivals (daughter -> day):", best['details']['discharge_arrival'])
    print("Discharge schedule (actual discharge day):", best['details']['discharge_schedule'])
    print("Waiting before load (days):", best['details']['W_load'])
    print("Waiting before discharge (days):", best['details']['W_disch'])
    print("Daughter demurrage cost:", best['details']['daughter_demurrage_cost'])
    print("Mother demurrage cost:", best['details']['mother_demurrage_cost'])
else:
    print("No feasible schedules under these constraints.")
