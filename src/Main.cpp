#include "ConsoleApp.h"
#include "AssessmentRepository.h"

int main()
{
	AssessmentRepository repository(".");
	ConsoleApp app(repository);
	return app.run();
}
