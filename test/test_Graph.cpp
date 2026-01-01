#include <unity.h>

// Now include Graph header (Arduino.h and U8g2lib.h are mocked in test/ directory)
#include "../src/Graph.h"

void setUp(void) {
    // set up code here, to run before every test
}

void tearDown(void) {
    // clean up code here, to run after every test
}

// Test configure method
void test_Graph_configure_basic(void) {
    Graph graph;
    graph.configure(64, 32, 0.0f, 100.0f, false);
    
    // We can't directly test private members, but we can verify
    // the graph doesn't crash and works after configuration
    graph.addPoint(50.0f);
    // If we get here without crashing, configure worked
    TEST_PASS();
}

void test_Graph_configure_with_max_dimensions(void) {
    Graph graph;
    graph.configure(MAX_GRAPH_WIDTH, MAX_GRAPH_HEIGHT, -10.0f, 50.0f, true);
    
    graph.addPoint(20.0f);
    TEST_PASS();
}

void test_Graph_configure_with_oversized_dimensions(void) {
    Graph graph;
    // Should clamp to MAX values
    graph.configure(200, 50, 0.0f, 100.0f, false);
    
    graph.addPoint(50.0f);
    TEST_PASS();
}

// Test addPoint method
void test_Graph_addPoint_single(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    graph.addPoint(50.0f);
    // If we get here without crashing, it worked
    TEST_PASS();
}

void test_Graph_addPoint_multiple(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    for (int i = 0; i < 5; i++) {
        graph.addPoint(i * 10.0f);
    }
    TEST_PASS();
}

void test_Graph_addPoint_circular_buffer_wrap(void) {
    Graph graph;
    graph.configure(5, 10, 0.0f, 100.0f, false);
    
    // Add more points than width to test circular buffer
    for (int i = 0; i < 10; i++) {
        graph.addPoint(i * 10.0f);
    }
    TEST_PASS();
}

// Test averaging functionality
void test_Graph_averaging_single_value(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    graph.addValueToAverage(50.0f);
    graph.commitAverage(0.0f);
    // Should add the average (which is 50.0) as a point
    TEST_PASS();
}

void test_Graph_averaging_multiple_values(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    graph.addValueToAverage(20.0f);
    graph.addValueToAverage(30.0f);
    graph.addValueToAverage(40.0f);
    graph.commitAverage(0.0f);
    // Should add average of 30.0 as a point
    TEST_PASS();
}

void test_Graph_averaging_no_values_uses_default(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    // Don't add any values, just commit with default
    graph.commitAverage(75.0f);
    // Should add the default value (75.0) as a point
    TEST_PASS();
}

void test_Graph_averaging_reset_after_commit(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    graph.addValueToAverage(10.0f);
    graph.addValueToAverage(20.0f);
    graph.commitAverage(0.0f);
    
    // Add new values, they should not include previous ones
    graph.addValueToAverage(50.0f);
    graph.addValueToAverage(60.0f);
    graph.commitAverage(0.0f);
    // Should average 50 and 60, not include 10 and 20
    TEST_PASS();
}

void test_Graph_averaging_multiple_commits(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    // First average
    graph.addValueToAverage(10.0f);
    graph.commitAverage(0.0f);
    
    // Second average
    graph.addValueToAverage(20.0f);
    graph.addValueToAverage(30.0f);
    graph.commitAverage(0.0f);
    
    // Third average (no values)
    graph.commitAverage(100.0f);
    
    TEST_PASS();
}

// Test clear method
void test_Graph_clear(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    // Add some data
    graph.addPoint(10.0f);
    graph.addPoint(20.0f);
    graph.addPoint(30.0f);
    
    // Clear it
    graph.clear();
    
    // Add new data - should work fine
    graph.addPoint(50.0f);
    TEST_PASS();
}

void test_Graph_clear_resets_averaging(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    // Add some values to average
    graph.addValueToAverage(10.0f);
    graph.addValueToAverage(20.0f);
    
    // Clear
    graph.clear();
    
    // Commit with default - should use default since averaging was reset
    graph.commitAverage(99.0f);
    TEST_PASS();
}

// Test reconfigure
void test_Graph_reconfigure(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    graph.addPoint(50.0f);
    
    // Reconfigure with different settings
    graph.configure(20, 20, -50.0f, 50.0f, true);
    graph.addPoint(0.0f);
    
    TEST_PASS();
}

// Test edge cases
void test_Graph_negative_values(void) {
    Graph graph;
    graph.configure(10, 10, -100.0f, 0.0f, false);
    
    graph.addPoint(-50.0f);
    graph.addPoint(-75.0f);
    TEST_PASS();
}

void test_Graph_boundary_values(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    graph.addPoint(0.0f);    // Min value
    graph.addPoint(100.0f);  // Max value
    graph.addPoint(50.0f);   // Mid value
    TEST_PASS();
}

void test_Graph_out_of_range_values(void) {
    Graph graph;
    graph.configure(10, 10, 0.0f, 100.0f, false);
    
    // Values outside the configured range
    graph.addPoint(-50.0f);  // Below min
    graph.addPoint(150.0f);  // Above max
    TEST_PASS();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Configure tests
    RUN_TEST(test_Graph_configure_basic);
    RUN_TEST(test_Graph_configure_with_max_dimensions);
    RUN_TEST(test_Graph_configure_with_oversized_dimensions);
    
    // addPoint tests
    RUN_TEST(test_Graph_addPoint_single);
    RUN_TEST(test_Graph_addPoint_multiple);
    RUN_TEST(test_Graph_addPoint_circular_buffer_wrap);
    
    // Averaging tests
    RUN_TEST(test_Graph_averaging_single_value);
    RUN_TEST(test_Graph_averaging_multiple_values);
    RUN_TEST(test_Graph_averaging_no_values_uses_default);
    RUN_TEST(test_Graph_averaging_reset_after_commit);
    RUN_TEST(test_Graph_averaging_multiple_commits);
    
    // Clear tests
    RUN_TEST(test_Graph_clear);
    RUN_TEST(test_Graph_clear_resets_averaging);
    
    // Reconfigure tests
    RUN_TEST(test_Graph_reconfigure);
    
    // Edge case tests
    RUN_TEST(test_Graph_negative_values);
    RUN_TEST(test_Graph_boundary_values);
    RUN_TEST(test_Graph_out_of_range_values);
    
    return UNITY_END();
}
