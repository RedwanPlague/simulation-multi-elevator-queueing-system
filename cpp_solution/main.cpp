#include "Config.h"
#include "Simulator.h"
#include "Statistics.h"

int main() {
    /* set_seed(); */

    string input_file = "input.txt";
    string output_file = "output.csv";

    Config config(input_file);
    ofstream fout;

    fout.open(output_file);

    fout << "No,";
    Statistics::print_headers(fout, config.elevator_count);

    int N = 10;
    for (int i = 1; i <= N; i++) {
        Simulator simulator(config);
        Statistics stats = simulator.run();
        fout << i << ",";
        stats.print_data(fout);
        cout << "simulation " << i << " done." << endl;
    }

    fout << "---,";
    int H = Statistics::print_headers(fout, config.elevator_count, true);

    fout.close();

    /* ************** */

    vector<double> avg(H);
    ifstream fin(output_file);

    string s;
    getline(fin, s);

    for (int i = 0; i < N; i++) {
        getline(fin, s, ',');
        for (auto &x : avg) {
            getline(fin, s, ',');
            x += stod(s);
        }
    }
    fin.close();

    /* ************** */

    fout.open(output_file, ios_base::app);
    fout << "Avg,";
    fout << fixed << setprecision(2);
    for (auto x : avg) {
        fout << x / N << ",";
    }
    fout.close();
}
