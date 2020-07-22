#! /bin/bash

echo Step1: running MinVar
./bin/MVRoot test_cases/unrooted.trees test_cases/MV_rooted.trees

echo Step2: running all-pairs tripRoot
./run_tripVote.sh test_cases/MV_rooted.trees test_cases/wMVvote_rooted.trees test_cases/MV_rooted.trees y
