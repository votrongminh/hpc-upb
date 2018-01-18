# Exercise: RushHour #

In the logic game [Rush Hour](http://www.thinkfun.com/products/rush-hour) cars are jammed in an intersection. The goal is to get a special car from one side of the playing field to the other.
Each car has a fixed orientation and can go forward/backward unless blocked by another car or the playing field border.

## Tasks ##

In this exercise we implement a (multi-threaded) brute force solver for this game. To do this efficiently we must globally keep track of which configurations have already been checked.

To simplify the programming, you are provided with a basic set of classes and methods that manage the game board and movements of the car. Your task will be implement the parallel game tree search using OpenMP tasking.

* Classes:
	- ```Car```: Models a single car
	- ```State```: Models a configuration of cars
	- ```StateManager```: Keeps track of the current best solution and of which States have already been checked.
* Functions
	- ```Check(...)```: checks whether a state fulfills the game's goal, recursively creates and checks follow-up configurations.
	- ```main(...)```: Sets up the initial state and calls ```Check(...)``` on it.

Note: To solve this exercise you only need to write in ```RushHour.cpp```, ```StateManager.h``` and ```StateManager.cpp``` and only to read the headers of the others.

### Task 0 ###

Draw the initial configuration built in the ```main()``` function.
Work out a shortest solution to compare your algorithm against.

### Task 1 ###

Implement the body of the ```Check(...)``` function. It should perform the following operations:

* Immediately return if the State to be analyzed took more steps to reach than our current best solution.
* Try to claim its state in the manager. If the state is already claimed by another task, immediately return.
* Check if the state is a winning state, if so enter its solution into the manager and return.
* Iterate over all the cars (```state.carCount()```), for each car create the two followup states
	 created by moving the respective car forward or backward. ```(state.move_car(...)``` returns such a followup state from a given car number and direction)
* Check whether the followup states created are legal states. If so recursively call ```Check(...)``` on them.


### Task 2 ###

Add OpenMP functionality to the solver by enclosing the main function's ```Check()``` call in a
```#omp parallel``` environment and a ```taskgroup```. Spawn the ```Check(...)``` function as a task.	Remember that the first ```Check(...)``` call only needs to be called once

* In ```Check()``` spawn more tasks for the recursion in a reasonable way.
* Use the ```default(none)```clause and explicitly state what a task can access and how it is accessed.
* Does this change the expected traversal of the possible configurations?

### Task 3 ###

StateManager is responsible for state that is shared across all tasks, yet lacks any precautions against data races.

* add a Mutex (```omp_lock_t from <omp.h>``` or ```std::Mutex from <mutex>```) and use it where necessary to make the program safe.

### Task D (bonus) ###

C++14 offers a reader/writer lock called ```std::shared_timed_mutex```, which allows multiple concurrent reads if locked with the proper function. Attempt to improve the efficiency of StateManager by locking as a reader whenever possible	and only locking as a writer when a write is necessary.

## Grading ##

A correct solution of this exercise will be awarded with 20 points.

## Hints ##

### Compiling and running the program on Oculus ###

Compilation:

``` shell
module add gcc/6.1.0
g++ -g -Wall -fopenmp -o RushHour Car.cpp State.cpp StateManager.cpp RushHour.cpp
```

Allocate an Occulus for interactive use. The following command allocates one node exclusively and sets the OpenMP environment variable to use 16 threads (one per core).

``` shell
ccsalloc -I --res=rset=ncpus=16,place=:excl
./RushHour
```
