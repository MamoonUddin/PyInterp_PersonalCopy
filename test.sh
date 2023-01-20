mkdir testcases
rm -rf testcases/*.txt

mkdir phase2
rm -rf phase2/*.txt

curl -o "testcases/in0#1.py" http://www2.cs.uh.edu/~dss/teaching/COSC4315/testcases/pyinterpreter-expressions/phase_1/in0[1-9].py
curl -o "testcases/in10.py" http://www2.cs.uh.edu/~dss/teaching/COSC4315/testcases/pyinterpreter-expressions/phase_1/in10.py

curl -o "phase2/in0#1.py" http://www2.cs.uh.edu/~dss/teaching/COSC4315/testcases/pyinterpreter-expressions/phase_2/in0[1-9].py
curl -o "phase2/in10.py" http://www2.cs.uh.edu/~dss/teaching/COSC4315/testcases/pyinterpreter-expressions/phase_2/in10.py
