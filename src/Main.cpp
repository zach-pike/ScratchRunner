#include <bits/stdc++.h>

#include "Scratch/Parse.hpp"
#include <iostream>

#include "ScratchRunner/Runner.hpp"

int main() {
    auto project = ParseProjectJSON("proj/project.json");

    Runner runner;
    runner.loadProject(project);
    runner.run();
}