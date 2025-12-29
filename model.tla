-- Formal model for CAPS processes
-- Using TLA+ or similar

---- MODULE CAPS ----
VARIABLE state
Init == state = "start"
Next == \/ state = "start" /\ state' = "running"
        \/ state = "running" /\ state' = "done"
Spec == Init /\ [][Next]_state
====