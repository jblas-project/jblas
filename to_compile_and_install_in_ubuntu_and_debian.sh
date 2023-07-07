echo "Use sdkmam for install : jdk, maven and ant."

sudo apt install apt-transport-https ca-certificates gnupg2 curl
sudo apt install gawk autoconf automake bison libgdbm-dev libncurses5-dev libsqlite3-dev libtool libyaml-dev sqlite3 libgmp-dev libreadline6-dev libssl-dev
sudo apt install libreadline6
sudo apt install libreadline
sudo apt install libreadline6-dev
sudo apt install libreadline-dev
sudo apt install gfortran libopenblas-serial-dev build-essential ruby

mvn clean
mvn compile
mvn package
mvn install