#include <iostream>
#include <cmath>
#include <tbb/flow_graph.h>


double f(double x) {
    return 1.0 / (1.0 + x * x);
}

int main() {
    const int num_intervals = 1000000; 
    const double width = 1.0 / num_intervals;

    double total_sum = 0.0;

    tbb::flow::graph graph;

    tbb::flow::function_node<int, double> calculate(graph, tbb::flow::unlimited,
        [=](int i) -> double {
            double x = (i + 0.5) * width;
            return f(x) * width;
        }
    );

    tbb::flow::function_node<double, tbb::flow::continue_msg> sum_up(graph, tbb::flow::serial,
        [&total_sum](double partial_sum) -> tbb::flow::continue_msg {
            total_sum += partial_sum;
            return tbb::flow::continue_msg();
        }
    );

    tbb::flow::make_edge(calculate, sum_up);

    for (int i = 0; i < num_intervals; ++i) {
        calculate.try_put(i);
    }

    graph.wait_for_all();

    std::cout << "Pi equals: " << 4.0 * total_sum << std::endl;

    return 0;
}
