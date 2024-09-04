# rtree
trivial rtree implementation to solve a simple puzzle

### problem statement

```
You are given a description of a robot's path. The robot begins at point (0,0) and performs 'n' commands. Each command moves the robot some distance up, down, left or right.
The robot will stop when it has performed all commands, or immediately when it returns to a point that it has already visited. Your task is to calculate the total distance the robot moves.

INPUT
The first input line has an integer 'n' : the number of commands.
After that, there are 'n' lines describing the commands. Each line has a character 'd' and an integer 'x': the robot moves the distance 'x' to the direction 'd'. Each direction is 'U' (up), 'D' (down), 'L' (left), or 'R' (right).
OUTPUT
Print the total distance the robot moves.
CONSTRAINTS
1 < n < 10^5
1 < x < 10^6

Example
INPUT:
5
U 2
R 3
D 1
L 5
U 2
OUTPUT:
9
```

### notes

There's an asymptotically efficient solution which breaks up the lines into horizontal and vertical then does a binary search over them. This solution is mainly interested in implementing the rtree. The rtree is very dependent on heuristics to perform well. A poorly chosen heuristic in rt_split will end up putting all the nodes to one side, so that search ends up walking all the nodes. 
