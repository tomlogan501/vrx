#!/bin/bash

# spawn_wamv.bash: A bash script to spawn a wamv model using gz.
#
# E.g.: ./spawn_wamv.bash /home/<username>/my_urdf.urdf

set -e
set -x

# Constants.
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NOCOLOR='\033[0m'

is_gzserver_running()
{
  if pgrep gzserver >/dev/null; then
    true
  else
    false
  fi
}

wait_until_gzserver_is_up()
{
  until is_gzserver_running
  do
    sleep 2s
  done

  while [[ "$(gz topic -l | wc -l)" -le 2 ]];do
    sleep 2s
  done

  sleep 2s
}

# Define usage function.
usage()
{
  echo "Usage: $0 -x <x> -y <y> -z <z> -R <R> -P <P> -Y <Y> --urdf <urdf> --model <model>"
  exit 1
}

# Call usage() function if arguments not supplied.
[[ $# -le 15 ]] && usage

POSITIONAL=()
while [[ $# -gt 0 ]]
do
  key="$1"
  
  case $key in
      -x)
      x="$2"
      shift # past argument
      shift # past value
      ;;
      -y)
      y="$2"
      shift # past argument
      shift # past value
      ;;
      -z)
      z="$2"
      shift # past argument
      shift # past value
      ;;
      -R)
      R="$2"
      shift # past argument
      shift # past value
      ;;
      -P)
      P="$2"
      shift # past argument
      shift # past value
      ;;
      -Y)
      Y="$2"
      shift # past argument
      shift # past value
      ;;
      --urdf)
      urdf="$2"
      shift # past argument
      shift # past value
      ;;
      --model)
      model="$2"
      shift # past argument
      shift # past value
      ;;
      *)    # unknown option
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
      ;;
  esac
  done
set -- "${POSITIONAL[@]}" # restore positional parameters


wait_until_gzserver_is_up
echo "gzserver is up"
gz model --model-name=$model --spawn-file=$urdf -x $x -y $y -z $z -R $R -P $P -Y $Y
