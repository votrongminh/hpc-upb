#include "StateManager.h"

#include <mutex>
#include <algorithm>

using namespace std;

void StateManager::enterSolution(const State& res){
	std::lock_guard<std::shared_timed_mutex> solLock(bestSolMutex);
	if(res.solutionSize() < best_solution_size){
		printf("Solution updated, size %d \n",res.solutionSize());
		best_solution = res;
		best_solution_size = res.solutionSize();
	}
}

bool StateManager::claim(const State& val){
		

	/*
		If we find that some other task has already claimed the exact configuration and reached it in as many or less steps,
		we return false so it won't be worked on anymore.
	*/

	std::shared_lock<std::shared_timed_mutex> readerLock(claimMutex);
	{
		auto val_it = state_set.find(val);
		auto end_state = state_set.cend();
	
		if(val_it != end_state && val.solutionSize()>=val_it->solutionSize()){
			readerLock.unlock();
			return false; //Someone else is already doing that / has already done that
		}
	}
	readerLock.unlock();

	//Two val elements are treated as equal, even if the steps to reach them are different. 
	std::lock_guard<std::shared_timed_mutex> writerLock(claimMutex); 
	{
		state_set.erase(val);
		state_set.insert(val);	
	}
	return true;
};


void StateManager::printBestSolution(){
	best_solution.printSolution();
}

