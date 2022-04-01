TMP_NAME=$1
SIM_NAME=$(awk '/-redir:sim/ {print $2}' < $1)

echo "$SIM_NAME"

cd bzip2 
$SIMPLESIM/simplesim-3.0/sim-outorder -config ../tmp.cfg bzip2_base.i386-m32-gcc42-nn dryer.jpg > /dev/null
echo "bzip2: $(grep sim_IPC $SIM_NAME)"
cd ..

cd equake 
$SIMPLESIM/simplesim-3.0/sim-outorder -config ../tmp.cfg equake_base.pisa_little < inp.in > inp.out > /dev/null
echo "equake: $(grep sim_IPC $SIM_NAME)"
cd ..

cd hmmer 
$SIMPLESIM/simplesim-3.0/sim-outorder -config ../tmp.cfg hmmer_base.i386-m32-gcc42-nn bombesin.hmm > /dev/null
echo "hmmer: $(grep sim_IPC $SIM_NAME)"
cd ..

cd mcf 
$SIMPLESIM/simplesim-3.0/sim-outorder -config ./../tmp.cfg mcf_base.i386-m32-gcc42-nn inp.in > /dev/null
echo "mcf: $(grep sim_IPC $SIM_NAME)"
cd ..

cd milc 
$SIMPLESIM/simplesim-3.0/sim-outorder -config ./../tmp.cfg milc_base.i386-m32-gcc42-nn < su3imp.in > /dev/null
echo "milc: $(grep sim_IPC $SIM_NAME)"
cd ..

cd sjeng 
$SIMPLESIM/simplesim-3.0/sim-outorder -config ./../tmp.cfg sjeng_base.i386-m32-gcc42-nn test.txt 
echo "sjeng: $(grep sim_IPC $SIM_NAME)"
cd ..
