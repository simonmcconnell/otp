%% Handwritten addition emitted by wx_gen_erl after the generated wxGrid methods.
-doc """
Assigns a virtual table to the grid. Call this instead of `createGrid/3`.

The grid takes ownership of `Table`, which must be a `m:wxGridTableBase`
created by `wxGridTableBase:new/4` and not already assigned to another grid.
""".
-spec setTable(wxGrid(), wxGridTableBase:wxGridTableBase()) -> boolean().
setTable(#wx_ref{type=ThisT}=This, #wx_ref{type=wxGridTableBase}=Table) ->
  ?CLASS(ThisT, wxGrid),
  wxe_util:queue_cmd(This, Table, ?get_env(), ?wxGrid_SetTable),
  wxe_util:rec(?wxGrid_SetTable).
