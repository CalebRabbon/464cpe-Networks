#Testing no input
./trace

#Testing input of non existent file
./trace 1
./trace fasdf

#Testing valid input
#./trace ./itests/ArpTest.pcap

for testInput in ./itests/*.pcap; do
   # Finds the *.pcap files and removes the "./itests" in front of the name
   just_name=${testInput#./itests/}

   # Runs my trace program with the name of each pcap file then outputs the
   # result to just_name.test
   ./trace ${testInput} > ./otests/${just_name}.test

   # diffs my resulting output file (.test file) to Prof Smith's .out file
   diff -q ./otests/${just_name}.test ./otests/${just_name}.out

done

#diff ./otests/UDPfile.pcap.test ./otests/UDPfile.pcap.out

