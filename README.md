*This project has been created as part of the 42 curriculum by gmateus-.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads. N coders
sit around a circular table with N USB dongles (one between each pair of
neighbours). A coder needs **two** dongles to compile, then debugs, then
refactors, and must start a new compile within `time_to_burnout` ms of the
start of the previous one, otherwise they burn out and the simulation stops.
Dongles have a cooldown after release and are granted according to a
scheduling policy (`fifo` or `edf`), using a hand-written binary heap.

The goal is to master mutexes, condition variables, deadlock/starvation
avoidance and precise time monitoring.

## Instructions

```
make                # builds ./codexion with cc -Wall -Wextra -Werror -pthread
make clean | fclean | re
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

- Times are in milliseconds, all arguments are mandatory, `scheduler` is
  exactly `fifo` or `edf`.
- Accepted values: integers only (no sign, no spaces, no overflow);
  `number_of_coders` in `[1, 2000]`, `time_to_burnout` and
  `number_of_compiles_required` >= 1, the others >= 0.
- Example: `./codexion 5 800 200 200 200 7 0 edf`
- Tests (optional, not graded): `sh tests/run_tests.sh` runs a battery of runs
  and validates every log with `tests/check_log.py` (state order, dongle mutual
  exclusion, cooldown, burnout deadline and burnout log precision).

Log format: `timestamp_ms X has taken a dongle | is compiling | is debugging |
is refactoring | burned out`. The timestamp is relative to the simulation start.

## Resources

- `man 3 pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`
- Dijkstra, *Hierarchical ordering of sequential processes* (dining philosophers)
- Coffman, Elphick, Shoshani, *System Deadlocks* (1971) - the four conditions
- Liu & Layland, *Scheduling Algorithms for Multiprogramming in a Hard
  Real-Time Environment* (1973) - Earliest Deadline First
- Cormen et al., *Introduction to Algorithms* - binary heaps
- ThreadSanitizer / AddressSanitizer documentation (used to validate the code)

**Use of AI:** an AI assistant (Claude) was used to discuss the design
(atomic dongle acquisition, scheduler semantics), to generate a first version
of the code, the log-checking script and this README, and to run the
sanitizer/test campaigns. Everything was reviewed and must be understood by
the authors before defence. *(Edit this paragraph so that it reflects your real use.)*

## Blocking cases handled

- **Deadlock (Coffman's conditions).** A coder never holds a dongle while
  waiting for another: it enqueues a request on both of its dongles and takes
  them **atomically**, only when it is at the head of *both* queues and both
  are free. This removes *hold and wait*, so no circular wait can form.
  Mutexes themselves are always locked in a fixed order (coder lock, then
  dongle locks by increasing index), so there is no lock-order inversion.
- **Starvation.** Every request gets a fixed priority key when created
  (`fifo`: arrival sequence number; `edf`: `last_compile_start +
  time_to_burnout`, ties broken by arrival sequence number, which is unique,
  so the order is fully deterministic). The same total order is used in both
  heaps, hence the waiting coder with the smallest key is always head of both
  its queues and gets served as soon as the holders release (bounded wait);
  nobody can overtake it. With `edf` the most urgent coder is always first.
- **Cooldown.** Releasing a dongle stores `free_at = now + dongle_cooldown`.
  A coder that is otherwise ready waits with `pthread_cond_timedwait` until
  the later `free_at` of its two dongles instead of busy-waiting.
- **Single coder.** Only one dongle exists; the coder takes it, can never
  compile and burns out at `time_to_burnout`.
- **Precise burnout detection.** A dedicated monitor thread checks every
  coder every 0.5 ms (`now - last_compile_start > time_to_burnout`), so the
  burnout line is printed ~1 ms after the real burnout (limit: 10 ms).
  `last_compile_start` is updated in the same critical section that grants
  the dongles, so there are no false positives.
- **Log serialization / no output after the end.** Each line is built in a
  buffer and written with a single `write` under `log_lock`. The stop flag
  lives under the same mutex, and the burnout line is printed in the same
  critical section that sets it, so nothing can be printed after it.
- **Clean termination.** On stop the monitor wakes every coder
  (`pthread_cond_signal` under each coder's lock); sleeps are done in 1 ms
  slices that check the stop flag. All threads are joined, all memory freed.
- **Symmetry at start.** All coders wait for a common start time; even
  numbered coders request 1 ms later so that non-adjacent coders (odd ones)
  are served first and can compile in parallel from the first round.
- **Thread creation / allocation failure.** The simulation is stopped and
  every created thread is joined; partial initialisation is cleaned up.

## Thread synchronization mechanisms

| Primitive | Where | Protects |
|---|---|---|
| `pthread_mutex_t` in each **dongle** | `t_dongle.lock` | `in_use`, `free_at`, and the dongle's priority queue (heap) |
| `pthread_mutex_t` + `pthread_cond_t` in each **coder** | `t_coder.lock/cond` | `last_start`, `compiles`, and the coder's wait for its turn |
| `log_lock` | `t_sim` | output (one `write` per line) **and** the `stop` flag (monitor state) |
| `seq_lock` | `t_sim` | the global arrival counter used for fifo order / edf tie-break |

**How a coder gets dongles** (`dongle.c`): push the request (same key) in both
heaps (one dongle lock at a time) -> lock own mutex -> `try_take`: lock both
dongles in index order, check "head of both queue AND both free AND cooldown
elapsed"; if yes pop both and mark them in use; else unlock and sleep on its
own condition variable (`pthread_cond_wait`, or `pthread_cond_timedwait` when
only a cooldown remains).

**How it is woken** (`release.c`): the releaser frees each dongle and sets
the cooldown under the dongle's mutex, reads the id of the new head of that
queue, **unlocks**, then locks that coder's mutex and signals its condition
variable. A waiter checks its condition while holding its own mutex up to the
atomic unlock inside `cond_wait`, and a waker must take the same mutex to
signal, so a wake-up cannot be lost (spurious ones are handled by the loop).

**Race conditions prevented:**
- Two coders cannot get the same dongle: `in_use` is changed only with the
  dongle mutex held, and both dongles are taken inside one critical section.
- A message can't be mixed with another or follow the burnout line: single
  `write` under `log_lock`, stop flag under the same lock.
- The monitor reads `last_start`/`compiles` with the coder's mutex; the coder
  writes them with it. A burnout can't be reported between "dongles granted"
  and "last_start updated" (same critical section).
- Lock hierarchy (no cycles): `coder.lock -> dongle.lock (ascending index)`,
  `coder.lock -> log_lock`. Dongle locks and `log_lock` never take a coder
  lock while held.

**Coder <-> monitor communication:** the monitor only reads shared state
under the same mutexes and raises the `stop` flag; coders poll it (`is_stopped`)
in every wait loop and the monitor wakes sleeping coders so they exit at once.

## Technical choices / known limits

- Scheduling is **strict per dongle**: a request is granted only to the head
  of the queue, as the subject demands. A waiting coder that is blocked
  keeps its place, so for very tight parameters (burnout close to the
  minimum feasible cycle) burnouts can still happen: that is a property of
  the parameters, not a synchronisation bug.
- Time uses `gettimeofday` (ms); `pthread_cond_timedwait` uses the same
  (realtime) clock.
