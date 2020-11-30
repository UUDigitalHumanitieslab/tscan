# BUILD

source lamachine-activate

sudo chown -R lamachine:lamachine /usr/local/src/tscan

if [ -e clam_custom.config.yml ]; then
    echo "Set custom clam"
    cp -f clam_custom.config.yml /usr/local/etc/
    lamachine-config clam_include /usr/local/etc/clam_custom.config.yml
else
    echo "No custom clam_include found"
fi

# Make the install.yml point to the right host
cd /usr/local/src/LaMachine
mv install.yml install.yml.bak
sed "s/hosts: localhost/hosts: develop/g" install.yml.bak > install.yml

# overwrite roles (needed for lamachine-update)
cp -R /deployment/roles/* /usr/local/src/LaMachine/roles

cd /usr/local/src/tscan
git add */.*
(git -c "user.name=lamachine" -c "user.email=lamachine@localhost" commit -m "Cannot work with uncommited changes")
temp_commit=$?
lamachine-add tscan
lamachine-update --only alpino,tscan
if [ $temp_commit -eq 0 ]; then
    echo "Undo temporary commit"
    git reset --soft HEAD~1
else
    echo "No local changes in repository"
fi

# clean up
rm -rf /usr/local/src/tscan/data
