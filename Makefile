prio:
	gcc queue.c pingpong.c pingpong-contab-prio.c -o p06-prio

no-prio:
	gcc queue.c pingpong.c pingpong-contab.c -o p06-no-prio
