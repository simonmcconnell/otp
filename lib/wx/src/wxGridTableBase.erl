%%
%% %CopyrightBegin%
%%
%% SPDX-License-Identifier: Apache-2.0
%%
%% Copyright 2026 Simon McConnell.
%%
%% Licensed under the Apache License, Version 2.0 (the "License");
%% you may not use this file except in compliance with the License.
%% You may obtain a copy of the License at
%%
%%     http://www.apache.org/licenses/LICENSE-2.0
%%
%% Unless required by applicable law or agreed to in writing, software
%% distributed under the License is distributed on an "AS IS" BASIS,
%% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%% See the License for the specific language governing permissions and
%% limitations under the License.
%%
%% %CopyrightEnd%
%%
-module(wxGridTableBase).
-moduledoc """
A virtual string table for `m:wxGrid`.

Cell values are requested from `GetValue` as the grid needs them. `SetValue`
must update the application's data source synchronously so a subsequent
`GetValue` returns the edited value. Callbacks run in the wx callback process
and must not make synchronous calls to other processes or to wx.

Pass the table to `wxGrid:setTable/2` instead of calling `wxGrid:createGrid/3`.
The grid takes ownership of the table. Keep the reference only while the grid
exists. Cell editors and colours can still be assigned using `m:wxGrid`.
""".
-include("wxe.hrl").
-export([new/4, setRowCount/2, setColCount/2]).
-export_type([wxGridTableBase/0]).

-type wxGridTableBase() :: wx:wx_object().

-spec new(non_neg_integer(), non_neg_integer(),
          fun((wxGridTableBase(), non_neg_integer(), non_neg_integer()) -> unicode:chardata()),
          fun((wxGridTableBase(), non_neg_integer(), non_neg_integer(), unicode:chardata()) -> term())) ->
          wxGridTableBase().
new(Rows, Cols, GetValue, SetValue)
  when is_integer(Rows), Rows >= 0, is_integer(Cols), Cols >= 0,
       is_function(GetValue, 3), is_function(SetValue, 4) ->
    GetBinary = fun(Table, Row, Col) ->
                    unicode:characters_to_binary(GetValue(Table, Row, Col))
                end,
    GetId = wxe_util:get_cbId(GetBinary),
    SetId = wxe_util:get_cbId(SetValue),
    wxe_util:queue_cmd(Rows, Cols, GetId, SetId, ?get_env(),
                       ?wxGridTableBase_new),
    wxe_util:rec(?wxGridTableBase_new).

-spec setRowCount(wxGridTableBase(), non_neg_integer()) -> ok.
setRowCount(#wx_ref{type=wxGridTableBase}=Table, Rows)
  when is_integer(Rows), Rows >= 0 ->
    wxe_util:queue_cmd(Table, Rows, ?get_env(), ?wxGridTableBase_SetRowCount),
    ok.

-spec setColCount(wxGridTableBase(), non_neg_integer()) -> ok.
setColCount(#wx_ref{type=wxGridTableBase}=Table, Cols)
  when is_integer(Cols), Cols >= 0 ->
    wxe_util:queue_cmd(Table, Cols, ?get_env(), ?wxGridTableBase_SetColCount),
    ok.
