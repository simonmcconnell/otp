/*
 * %CopyrightBegin%
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright Ericsson AB 2008-2026. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * %CopyrightEnd%
 */

#include <wx/wx.h>
#include "wxe_impl.h"
#include "wxe_return.h"
#include "wxe_events.h"
#include "wxe_gl.h"
#include "gen/wxe_macros.h"
#include "gen/wxe_derived_dest.h"


/* ****************************************************************************
 * CallbackData *
 * ****************************************************************************/

wxeEvtListener::wxeEvtListener(ErlNifPid caller, int req, ERL_NIF_TERM req_type,
			       int funcb, int skip_ev, wxeErlTerm * userData,
			       wxe_me_ref *mr)
  : wxEvtHandler()
{
  me_ref=mr;
  listener = caller;
  obj = req;
  fun_id = funcb;
  class_name = req_type;
  skip = skip_ev;
  user_data = userData;
}

wxeEvtListener::~wxeEvtListener() {
  // enif_fprintf(stderr, "CBD Deleting %p %T\r\n", this, class_name); fflush(stderr);
  if(user_data) {
    delete user_data;
  }
  ptrMap::iterator it;
  it = ((WxeApp *)wxTheApp)->ptr2ref.find(this);
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(it != ((WxeApp *)wxTheApp)->ptr2ref.end() && memenv) {
    wxeRefData *refd = it->second;
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    rt.send(enif_make_tuple4(rt.env,
                             rt.make_atom("wx_delete_cb"),
                             rt.make_int(fun_id),
                             rt.make_ref(refd->ref, "wxeEvtListener"),
                             rt.make_ref(obj, class_name)));
  }
  ((WxeApp *)wxTheApp)->clearPtr(this);
}

void wxeEvtListener::forward(wxEvent& event)
{
  if(me_ref->memenv)
    sendevent(&event, (wxeMemEnv *)me_ref->memenv);
}

/* *****************************************************************/
/* Printing special */

EwxPrintout::~EwxPrintout() {
  clear_cb(me_ref, onPrintPage);
  clear_cb(me_ref, onPreparePrinting);
  clear_cb(me_ref, onBeginPrinting);
  clear_cb(me_ref, onEndPrinting);
  clear_cb(me_ref, onBeginDocument);
  clear_cb(me_ref, onEndDocument);
  clear_cb(me_ref, hasPage);
  clear_cb(me_ref, getPageInfo);

  ((WxeApp *)wxTheApp)->clearPtr(this);
}

bool EwxPrintout::OnBeginDocument(int startPage, int endPage)
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onBeginDocument && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 2,
                                       rt.make_int(startPage),
                                       rt.make_int(endPage));
    rt.send_callback(onBeginDocument, this, "wxPrintout", args);

    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    int ret_value;
    if(cb && enif_get_int(cb->env, cb->args[0], &ret_value)) {
      delete cb;
      return ret_value;
    }
    if(cb) delete cb;
  }
  return wxPrintout::OnBeginDocument(startPage,endPage);
}

void EwxPrintout::OnEndDocument()
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onEndDocument && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 0);
    rt.send_callback(onEndDocument, this, "wxPrintout", args);

  } else {
    wxPrintout::OnEndDocument();
  }
}

void EwxPrintout::OnBeginPrinting()
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onBeginPrinting && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 0);
    rt.send_callback(onBeginPrinting, this, "wxPrintout", args);
  } else {
    wxPrintout::OnBeginPrinting();
  }
}

void EwxPrintout::OnEndPrinting()
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onEndPrinting && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 0);
    rt.send_callback(onEndPrinting, this, "wxPrintout", args);
  } else {
    wxPrintout::OnEndPrinting();
  }
}

void EwxPrintout::OnPreparePrinting()
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onPreparePrinting && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 0);
    rt.send_callback(onPreparePrinting, this, "wxPrintout", args);
  } else {
    wxPrintout::OnPreparePrinting();
  }
}

bool EwxPrintout::HasPage(int page)
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(hasPage && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 1, rt.make_int(page));
    rt.send_callback(hasPage, this, "wxPrintout", args);
    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    int ret_value;
    if(cb && enif_get_int(cb->env, cb->args[0], &ret_value)) {
      delete cb;
      return ret_value;
    }
    if(cb) delete cb;
  }
  return wxPrintout::HasPage(page);
}

bool EwxPrintout::OnPrintPage(int page)
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 1, rt.make_int(page));
    rt.send_callback(onPrintPage, this, "wxPrintout", args);
    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    int ret_value;
    if(cb && enif_get_int(cb->env, cb->args[0], &ret_value)) {
      delete cb;
      return ret_value;
    }
    if(cb) delete cb;
  }
  return FALSE;
}

void EwxPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(getPageInfo && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 0);
    rt.send_callback(getPageInfo, this, "wxPrintout", args);
    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    int tpl_sz;
    const ERL_NIF_TERM *tpl;
    if(cb
       && enif_get_tuple(cb->env, cb->args[0], &tpl_sz, &tpl)
       && tpl_sz == 4
       && enif_get_int(cb->env, tpl[0], minPage)
       && enif_get_int(cb->env, tpl[1], maxPage)
       && enif_get_int(cb->env, tpl[2], pageFrom)
       && enif_get_int(cb->env, tpl[3], pageTo)
       ) {
      delete cb;
      return;
    }
    if(cb) delete cb;
  }
  wxPrintout::GetPageInfo(minPage, maxPage, pageFrom, pageTo);
}

/* *****************************************************************/
// Grid table whose values are supplied by Erlang instead of wxGridStringTable.

class EwxGridTable : public wxGridTableBase {
public:
  EwxGridTable(int rows, int cols, int getValue, int setValue, wxe_me_ref *ref)
    : m_rows(rows), m_cols(cols), m_getValue(getValue), m_setValue(setValue),
      m_ref(ref) {}

  ~EwxGridTable() override {
    clear_cb(m_ref, m_getValue);
    clear_cb(m_ref, m_setValue);
    ((WxeApp *)wxTheApp)->clearPtr(this);
  }

  int GetNumberRows() override { return m_rows; }
  int GetNumberCols() override { return m_cols; }

  wxString GetValue(int row, int col) override {
    wxeMemEnv *memenv = (wxeMemEnv *)m_ref->memenv;
    if (!memenv) return wxEmptyString;

    wxeReturn rt(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 2,
                                      rt.make_int(row), rt.make_int(col));
    rt.send_callback(m_getValue, this, "wxGridTableBase", args);
    wxeCommand *cb = ((WxeApp *)wxTheApp)->cb_return;
    ErlNifBinary bin;
    wxString value;
    if (cb && enif_inspect_binary(cb->env, cb->args[0], &bin))
      value = wxString(bin.data, wxConvUTF8, bin.size);
    if (cb) delete cb;
    return value;
  }

  void SetValue(int row, int col, const wxString& value) override {
    wxeMemEnv *memenv = (wxeMemEnv *)m_ref->memenv;
    if (!memenv) return;

    wxeReturn rt(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 3, rt.make_int(row),
                                      rt.make_int(col), rt.make(value));
    rt.send_callback(m_setValue, this, "wxGridTableBase", args);
    wxeCommand *cb = ((WxeApp *)wxTheApp)->cb_return;
    if (cb) delete cb;
  }

  void SetRowCount(int rows) {
    const int old = m_rows;
    m_rows = rows;
    if (wxGrid *grid = GetView()) {
      if (rows > old)
        grid->ProcessTableMessage(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
                                  rows - old);
      else if (rows < old)
        grid->ProcessTableMessage(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED,
                                  rows, old - rows);
    }
  }

  void SetColCount(int cols) {
    const int old = m_cols;
    m_cols = cols;
    if (wxGrid *grid = GetView()) {
      if (cols > old)
        grid->ProcessTableMessage(this, wxGRIDTABLE_NOTIFY_COLS_APPENDED,
                                  cols - old);
      else if (cols < old)
        grid->ProcessTableMessage(this, wxGRIDTABLE_NOTIFY_COLS_DELETED,
                                  cols, old - cols);
    }
  }

private:
  int m_rows;
  int m_cols;
  int m_getValue;
  int m_setValue;
  wxe_me_ref *m_ref;
};

void wxGridTableBase_new(WxeApp *app, wxeMemEnv *memenv, wxeCommand& Ecmd)
{
  ErlNifEnv *env = Ecmd.env;
  ERL_NIF_TERM *argv = Ecmd.args;
  int rows, cols, getValue, setValue;
  if (!enif_get_int(env, argv[0], &rows) || rows < 0) Badarg("rows");
  if (!enif_get_int(env, argv[1], &cols) || cols < 0) Badarg("cols");
  if (!enif_get_int(env, argv[2], &getValue)) Badarg("getValue");
  if (!enif_get_int(env, argv[3], &setValue)) Badarg("setValue");
  EwxGridTable *table = new EwxGridTable(rows, cols, getValue, setValue,
                                        memenv->me_ref);
  app->newPtr(table, 1, memenv);
  wxeReturn rt(memenv, Ecmd.caller, true);
  rt.send(rt.make_ref(app->getRef(table, memenv), "wxGridTableBase"));
}

void wxGrid_SetTable(WxeApp *app, wxeMemEnv *memenv, wxeCommand& Ecmd)
{
  ErlNifEnv *env = Ecmd.env;
  ERL_NIF_TERM *argv = Ecmd.args;
  wxGrid *grid = (wxGrid *)memenv->getPtr(env, argv[0], "grid");
  EwxGridTable *table = (EwxGridTable *)memenv->getPtr(env, argv[1], "table");
  if (!grid || !table || table->GetView()) Badarg("table");
  const bool result = grid->SetTable(table, true);
  wxeReturn rt(memenv, Ecmd.caller, true);
  rt.send(rt.make_bool(result));
}

void wxGridTableBase_SetRowCount(WxeApp *app, wxeMemEnv *memenv, wxeCommand& Ecmd)
{
  ErlNifEnv *env = Ecmd.env;
  ERL_NIF_TERM *argv = Ecmd.args;
  EwxGridTable *table = (EwxGridTable *)memenv->getPtr(env, argv[0], "table");
  int rows;
  if (!table || !enif_get_int(env, argv[1], &rows) || rows < 0)
    Badarg("rows");
  table->SetRowCount(rows);
}

void wxGridTableBase_SetColCount(WxeApp *app, wxeMemEnv *memenv, wxeCommand& Ecmd)
{
  ErlNifEnv *env = Ecmd.env;
  ERL_NIF_TERM *argv = Ecmd.args;
  EwxGridTable *table = (EwxGridTable *)memenv->getPtr(env, argv[0], "table");
  int cols;
  if (!table || !enif_get_int(env, argv[1], &cols) || cols < 0)
    Badarg("cols");
  table->SetColCount(cols);
}

// ListCtrl with callbacks for VIRTUAL_TABLES

wxString EwxListCtrl::OnGetItemText(long item, long col) const {
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onGetItemText && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 2, rt.make_int(item), rt.make_int(col));
    rt.send_callback(onGetItemText, (wxObject *)this, "wxListCtrl", args);

    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    ErlNifBinary bin;
    if(cb && enif_inspect_binary(cb->env, cb->args[0], &bin)) {
      wxString str = wxString(bin.data, wxConvUTF8, bin.size);
      delete cb;
      return str;
    }
    if(cb) delete cb;
    return wxT("OnGetItemText must return a string");
  }
  return wxT("OnGetItemText not defined");
}

wxListItemAttr* EwxListCtrl::OnGetItemAttr(long item) const {
  wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onGetItemAttr && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 1, rt.make_int(item));
    rt.send_callback(onGetItemAttr, (wxObject *) this, "wxListCtrl",args);
    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    if(cb) {
      wxListItemAttr * result = (wxListItemAttr *) memenv->getPtr(cb->env, cb->args[0], "CB item");
      delete cb;
      return result;
    }
  }
  return NULL;
}

int EwxListCtrl::OnGetItemImage(long item) const {
  return OnGetItemColumnImage(item, 0);
}

int EwxListCtrl::OnGetItemColumnImage(long item, long col) const {
    wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
  if(onGetItemColumnImage && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list(rt.env, 2, rt.make_int(item), rt.make_int(col));
    rt.send_callback(onGetItemColumnImage, (wxObject *) this, "wxListCtrl",args);

    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    int ret_value;
    if(cb && enif_get_int(cb->env, cb->args[0], &ret_value)) {
      delete cb;
      return ret_value;
    }
    if(cb) delete cb;
  }
  return -1;
}

EwxListCtrl::~EwxListCtrl() {
  clear_cb(me_ref, onGetItemText);
  clear_cb(me_ref, onGetItemAttr);
  clear_cb(me_ref, onGetItemColumnImage);
  ((WxeApp *)wxTheApp)->clearPtr(this);
}

/* ****************************************************************************
 * wxListCtrlCompare wrapper
 * ****************************************************************************/

int wxCALLBACK wxEListCtrlCompare(wxeIntPtr item1, wxeIntPtr item2, wxeIntPtr callbackInfoPtr)
{
  callbackInfo * cbi = (callbackInfo *)callbackInfoPtr;
  wxeMemEnv *memenv = (wxeMemEnv *) cbi->me_ref->memenv;
  if(memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM args = enif_make_list2(rt.env,rt.make_int(item1),rt.make_int(item2));
    rt.send_callback(cbi->callbackID, args);

    wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
    int ret_value;
    if(cb && enif_get_int(cb->env, cb->args[0], &ret_value)) {
      delete cb;
      return ret_value;
    }
    if(cb) delete cb;
  }
  return 0;
}


/* *****************************************************************/
// TaskBarIcon with callbacks for VIRTUAL_TABLES

wxMenu* EwxTaskBarIcon::CreatePopupMenu() {
  if(createPopupMenu) {
    wxeMemEnv *memenv = (wxeMemEnv *) me_ref->memenv;
    if(memenv) {
      wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
      ERL_NIF_TERM args = enif_make_list(rt.env, 0);
      rt.send_callback(createPopupMenu, args);

      wxeCommand *cb = ((WxeApp *) wxTheApp)->cb_return;
      wxMenu * ret_value;
      if(cb && (ret_value = (wxMenu *) memenv->getPtr(cb->env, cb->args[0], "menu"))) {
        delete cb;
        return ret_value;
      }
      if(cb) delete cb;
    }
  }
  return NULL;
}


// tools

void clear_cb(wxe_me_ref *mr, int callback)
{
  if(!mr) return;
  wxeMemEnv *memenv = (wxeMemEnv *) mr->memenv;
  if(callback > 0 && memenv) {
    wxeReturn rt = wxeReturn(memenv, memenv->owner, false);
    ERL_NIF_TERM cb_msg =
      enif_make_tuple2(rt.env,
                       rt.make_atom("wx_delete_cb"),
                       rt.make_int(callback));
    rt.send(cb_msg);
  }
}
