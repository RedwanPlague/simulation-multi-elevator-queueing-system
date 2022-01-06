#include "Config.h"
#include "Simulator.h"
#include "Statistics.h"

int main() {
    set_seed();

    Config config("input.txt");

    ofstream fout("output.csv");

    fout << "No,";
    Statistics::print_headers(fout, config.elevator_count);

    int N = 10;
    Statistics avg;
    for (int i = 1; i <= N; i++) {
        Simulator simulator(config);
        Statistics stats = simulator.run();
        fout << i << ",";
        stats.print_data(fout);
        cout << "simulation " << i << " done." << endl;
        avg = (i == 1 ? stats : avg + stats);
    }

    fout << "---,";
    Statistics::print_headers(fout, config.elevator_count, true);

    avg = avg / N;
    fout << "Avg,";
    avg.print_data(fout);

    fout.close();
}
