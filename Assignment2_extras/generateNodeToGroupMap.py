with open("./helper_scripts/nodefile.txt") as file:
    data = file.read()
    nodeToGroupMap = dict()
    for groupNumber, line in enumerate(data.split('\n')):
        for node in line.split(','):
            nodeToGroupMap[int(node[5:])] = groupNumber

nodeToGroupArray = [0]*(max(nodeToGroupMap.keys())+1)
for key in nodeToGroupMap:
    nodeToGroupArray[key] = nodeToGroupMap[key]

print('int nodeToGroupMap['+str(max(nodeToGroupMap.keys())+1)+'] = {'+', '.join(str(x) for x in nodeToGroupArray)+'}')
