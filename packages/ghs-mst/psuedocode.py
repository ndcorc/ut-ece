Psuedocode executed at each node (N)

1. Response to clicking node N (wakes it up):
	wakeup(N)

2. wakeup(N):
	MWOE (minimum weight outgoing edge) of N (connects to node M)
	MWOE.state = BRANCH
	N.level = 0
	N.state = FOUND
	find_count = 0
	send: Connect(N.level) --(MWOE)--> M

3. receive Connect(src.level):
	E = edge(src, N)
	if N.state == SLEEPING:
		wakeup(N)
	if src.level < N.level:
		E.state = BRANCH
		N.fragment = E.weight
		send: Initiate(N.level, N.fragment, N.state) --(E)--> src
		if (N.state == SEARCHING):
			find_count ++
	else:
		if E.state == BASIC:
			messageQueue.addLast(Connect())
		else:
			send: Initiate(N.level+1, E.weight, SEARCHING) --(E)--> src

4. receive Initiate(src.level, src.fragment, src.state):
	E = edge(src, N)
	N.level = src.level
	N.fragment = src.fragment
	N.state = src.state
	in_branch = E
	best_edge = NULL
	best_weight = -1 (infinity)
	for adjacent_edge i in adjacent_edges:
		if i != E and i.state == BRANCH:
			(i connects N to M)
			send: Initiate(src.level, src.fragment, src.state) --i-> M
			if src.state == SEARCHING:
				find_count ++
	if src.state == SEARCHING:
		test()

5. test():
	if there is a BASIC edge in adjacent_edges(N):
		for edge in basic_adjacent_edges(N): (adjacent edges in state BASIC)
			test_edge = MWOE_BASIC(N) (MWOE of BASIC edges)
			(test_edge connects N to M)
			send: Test(N.level, N.fragment) --test_edge--> M
	else:
		test_edge = NULL
		report()

6. receive Test(src.level, src.fragment):
	E = edge(src, N)
	if N.state == SLEEPING:
		wakeup()
	if src.level > N.level:
		messageQueue.addLast(Test())
	else if src.fragment != N.fragment:
		send: Accept() --E--> src
	else:
		if E.state == BASIC:
			E.state = REJECTED
		if test_edge != E:
			send: REJECT() --E--> src
		else:
			test()

7. receive Accept(): (src-->N)
	E = edge(src, N)
	test_edge = NULL
	if E.weight < best_weight:
		best_edge = E
		best_weight = E.weight
	report()

8. receive Reject(): (src-->N)
	E = edge(src, N)
	if E.state == BASIC:
		E.state = REJECTED
	test()

9. report():
	if (find_count == 0) and (test_edge == NULL):
		N.state = FOUND
		(in_branch connects N to M)
		send: Report(best_weight) --in_branch--> M

10. receive Report(src.weight):
	E = edge(src, N)
	if E != in_branch:
		find_count--
		if src.weight < best_weight:
			best_weight = src.weight
			best_edge = E
		report()
	else if N.state == SEARCHING:
		messageQueue.addLast(Report())
	else if src.weight > best_weight:
		change_core()
	else if src.weight == best_weight == -1:
		HALT

11. change_core():
	if best_edge.state == BRANCH:
		(best_edge connects N to M)
		send: ChangeCore() --best_edge--> M
	else:
		send: Connect(N.level) --best_edge--> M
		best_edge.state = BRANCH

12. receive ChangeCore():
	change_core()







