#! /usr/bin/env bash
set -o errexit
set -o nounset
set -o pipefail

make
alias msh='./bin/msh'