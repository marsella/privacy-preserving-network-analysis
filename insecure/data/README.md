Input files have four types of lines:

- `i` id
- `r` cash reserves 
- `d` debt (debtor's id, amount)
- `c` credit (ower id, amount)

If there are multiple id lines, the last will be used. If the id is larger than
the total number of banks, the code will fail.

If there are multiple reserve lines, the last will be used.

Debt and credit amounts are stored as unsigned integers. They should be nonnegative.

The files generated should be consistent  (that is, if A owes a debt to B, B should have a credit in the same amount from A)

File names should be formatted as `bank<id>.dat`.

