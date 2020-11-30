source lamachine-activate

if [ ! -z "$PYTHONPATH" ]; then
    OLDPYTHONPATH="$PYTHONPATH"
    export PYTHONPATH=""
fi

# Force updating the lamachine-core
# (to update the start webserver script and CLAM configuration)
cd "/usr/local/src/LaMachine"
echo "---
- hosts: develop
  roles: [ lamachine-core ]
" > "install.tmp.yml"

ansible-playbook -i "hosts.ini" "install.tmp.yml" -v --tags "webserver-start,webserver-clam-base" --extra-vars "ansible_python_interpreter='$(which python3)'" 2>&1
rc=${PIPESTATUS[0]}

if [ $rc -eq 0 ]; then
    echo "Applied configuration!"
else
    echo "WARNING: applying configuration failed!"
fi

if [ ! -z "$OLDPYTHONPATH" ]; then
    export PYTHONPATH="$OLDPYTHONPATH"
fi

exit $rc
