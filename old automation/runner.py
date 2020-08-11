#runner.py by Griffin

import os
import subprocess
import itertools
import json
import secrets

def _write_json(data, name, token, index):
    #print(name, token, index)
    path = os.path.join(".", "runs", (f"set_{name}_{token}" if name else f"set_{token}"), f"data_{index:04}", "logs")
    os.system(f"mkdir -p {path}")
    with open(os.path.join(path, "input.json"), "w") as input_file:
        json.dump(data, input_file)
    return os.path.abspath(path), 4

def _run_sim(data, name, token, index):
    path, depth = _write_json(data, name, token, index)
    if os.name == 'nt':
        return subprocess.Popen(f"cd {path} && {os.path.join(*itertools.repeat('..', depth), 'bin', 'hoya.exe')} 'input.json' > cout.log 2> cerr.log", shell=True), path, index
    else:
        with open(os.path.join(path, 'cout.log'), 'w') as cout, open(os.path.join(path, 'cerr.log'), 'w') as cerr:
            return subprocess.Popen([os.path.join(*itertools.repeat("..", depth), 'bin', 'hoya'), 'input.json'], cwd=path, stdout=cout, stderr=cerr), path, index

def _run_next(name, data_set):
    token = secrets.token_hex(16)
    for index, data in enumerate(data_set):
        yield _run_sim(data, name, token, index)

def run_sim_batched(data_set, batch_size = 1, name = None):
    batch_size = int(0 if batch_size < 0 else batch_size)
    sentinel = object()
    sims = _run_next(name, data_set)

    if batch_size == 0:
        for p in sims:
            p[0].wait()
            yield p[1]
    elif batch_size == 1:
        last_sim = None
        for p in sims:
            if last_sim:
                yield last_sim[1]
            p[0].wait()
            last_sim = p
        yield last_sim[1]
    else:
        last_returned = -1
        active_p = []
        finished_p = []
        data_finished = False
        while not data_finished:
            if len(active_p) < batch_size:
                new_p = next(sims, sentinel)
                if new_p is sentinel:
                    data_finished = True
                else:
                    active_p.append(new_p)
            else:
                any_gaps = False;
                for p in active_p:
                    if p[0].poll() is not None:
                        finished_p.append(p)
                        any_gaps = True
                        active_p.remove(p)
                if not any_gaps:
                    finished_p.sort(key=lambda tup: tup[2])
                    if len(finished_p) > 0 and finished_p[0][2] == last_returned+1:
                        #print('i')
                        done_p = finished_p.pop(0)
                        last_returned = done_p[2]
                        yield done_p[1]
                    else:
                        active_p[0][0].wait()
        for p in active_p:
            p[0].wait()
            active_p.remove(p)
            finished_p.append(p)

        finished_p.sort(key=lambda tup: tup[2])
        for p in finished_p:
            yield p[1]

def run_sim_once(data, name = None):
    return list(run_sim_batched([data], 1, name))[0];

#cerr.log  cout.log  input.json  pandemic_hoya_outputs.txt  pandemic_hoya_state.txt
