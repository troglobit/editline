#!/bin/sh
# Drive editline in a real terminal (tmux) and check that Ctrl-E on a line
# whose length is an exact multiple of the terminal width lands the cursor on
# the line's last row -- not the empty row below it, which scrolls the whole
# line up one row.  Exits 77 (SKIP) when tmux is unavailable or unusable, so
# the suite still runs on headless machines and in CI.

command -v tmux >/dev/null 2>&1 || exit 77

eltty="$(pwd)/eltty"		# automake runs each test from the build dir
[ -x "$eltty" ] || exit 77

sock="editline-test-$$"
sess=wrap
tm="tmux -L $sock"

cleanup() { $tm kill-server >/dev/null 2>&1; }
trap cleanup EXIT

# 10-column x 8-row pane running the helper; a server that won't start (no
# /tmp, headless restrictions, ...) is a skip, not a failure.
$tm new-session -d -s "$sess" -x 10 -y 8 "$eltty" 2>/dev/null || exit 77

# 20 characters == exactly two rows of 10.  send-keys queues into the pty even
# before readline() is ready, so just poll until the line has rendered (only
# integer sleeps -- fractional sleep is not portable).  Give up -> skip.
$tm send-keys -t "$sess" "01234567890123456789" 2>/dev/null || exit 77
i=0
while [ "$i" -lt 10 ]; do
	$tm capture-pane -t "$sess" -p 2>/dev/null | grep -q 0123456789 && break
	i=$((i + 1))
	sleep 1
done
[ "$i" -lt 10 ] || exit 77

# Jump home, jump to end, and let the cursor moves settle.
$tm send-keys -t "$sess" C-a 2>/dev/null || exit 77
$tm send-keys -t "$sess" C-e 2>/dev/null || exit 77
sleep 1

rows=$($tm capture-pane -t "$sess" -p 2>/dev/null | grep -c .)
cy=$($tm display-message -t "$sess" -p '#{cursor_y}' 2>/dev/null)
[ -n "$rows" ] && [ -n "$cy" ] || exit 77

want=$((rows - 1))
if [ "$cy" -eq "$want" ]; then
	echo "PASS wrap-tmux          Ctrl-E cursor on last row (y=$cy, $rows rows)"
	exit 0
fi

echo "FAIL wrap-tmux          Ctrl-E cursor on row $cy, expected $want (scrolled)" >&2
exit 1
