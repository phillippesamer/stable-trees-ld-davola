#include "io.h"
#include "model.h"
#include "ldda.h"

#include <cstdlib>
#include <sys/time.h>       // for 'gettimeofday()'

using namespace std;

static struct timeval *clock_start;

void start_timer()
{
    // current clock time
    clock_start = (struct timeval *) malloc(sizeof(struct timeval) );
    gettimeofday(clock_start, 0);
}

void get_timer()
{
    struct timeval *clock_stop;
    clock_stop = (struct timeval *) malloc( sizeof(struct timeval) );
    gettimeofday(clock_stop, 0);

    unsigned long clock_time = 1.e6 * (clock_stop->tv_sec - clock_start->tv_sec) +
                                      (clock_stop->tv_usec - clock_start->tv_usec);

    printf( "main() says: runtime in seconds\n%.4f\n" , ((double)clock_time / (double)1.e6) );

    free(clock_stop);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "usage: \t" << argv[0] << " [input file]" << endl << endl;
        return 0;
    }

    IO* instance = new IO();
    
    // parse given input file and look for errors in it
    if (instance->parse_gcclib(argv[1]) == false)
    {
        cout << "unable to parse input file" << endl;
        delete instance;
        return 0;
    }

    Model *model = new Model(instance);
    //model->solve(true);

    /*
    cout << endl;
    for (long i=0; i < instance->num_edges(); i++)
    {
        pair<ModelStatus,double> probing = model->probe_var(i,1);
        if (probing.first == AT_OPTIMUM)
        {
            cout << "probing var x[" << i << "] = 1 gives ObjVal=" << probing.second 
            << " (runtime: " << model->runtime() << " s)" << endl;
        }
        else if (probing.first == IS_INFEASIBLE)
        {
            cout << "probing var x[" << i << "] = 1 gives an infeasible model" 
            << " (runtime: " << model->runtime() << " s)" << endl;
        }
    }
    cout << endl;
    */

    LDDA *lagrangean = new LDDA(instance, model);
    start_timer();
    lagrangean->dual_ascent();
    get_timer();

    // clean up
    free(clock_start);
    delete lagrangean;
    delete instance;
    delete model;

    return 0;
}
