
ssh linprog1 'rm -rf temp* ; pkill -u chetty'
ssh linprog2 'rm -rf temp* ; pkill -u chetty'
ssh linprog3 'rm -rf temp* ; pkill -u chetty'
ssh linprog4 'rm -rf temp* ; pkill -u chetty'
ssh program1 'rm -rf temp* ; pkill -u chetty'
ssh program2 'rm -rf temp* ; pkill -u chetty'
ssh program3 'rm -rf temp* ; pkill -u chetty'
ssh program4 'rm -rf temp* ; pkill -u chetty'
ssh shell 'rm -rf temp* ; pkill -u chetty'

