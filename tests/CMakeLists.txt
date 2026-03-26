add_executable(unit_tests 
    test_anomaly_detector.cpp
)

# Link our core logic library AND Google Test
target_link_libraries(unit_tests PRIVATE 
    app_core 
    GTest::gtest_main 
    GTest::gmock
)

# Register with CTest
add_test(NAME AnomalyTests COMMAND unit_tests)