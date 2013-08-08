FILES="./*"
script_rc=0
failures=""
for f in $FILES
  do 
  TEST_DIR=${f}/tests
  echo $TEST_DIR
  if [ -d "$TEST_DIR" ]; then
    pushd $TEST_DIR
    for g in test_*.py
      do
      echo "run test$f"
      python $g
      rc=$?
      if [[ $rc != 0 ]] ; then
        script_rc=$rc
        failures="$failures $g"        
      fi
    done
    popd
  fi
done

echo "summary:$failures"

exit $script_rc
