#include "application.h"
#include "game.h"

#ifndef _WIN32
#include <sys/resource.h>

void set_stack_size(const rlim_t size);

#endif

int main(int argc, char** argv){

#ifndef _WIN32
	set_stack_size(128 * 1024 * 1024); //32MB must be enough, right?
#endif

	Application app;

	if (app.initialize(argc, argv)) {
		app.run();
	}

    return 0;
}

#ifndef _WIN32

void set_stack_size(const rlim_t size){

	struct rlimit rl;
	int result;

	result = getrlimit(RLIMIT_STACK, &rl);

	if (result == 0){
		if (rl.rlim_cur < size){

			rl.rlim_cur = size;
			result = setrlimit(RLIMIT_STACK, &rl);

			if (result != 0){
				std::cout << "Failed to increase the stack size, CubeExplorer might not run properly." << std::endl;
			}
		}
	}

}

#endif
