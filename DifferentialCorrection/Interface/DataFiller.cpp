// Flow Vector Correction Framework
//
// Copyright (C) 2018  Lukas Kreis, Ilya Selyuzhenkov
// Contact: l.kreis@gsi.de; ilya.selyuzhenkov@gmail.com
// For a full list of contributors please see docs/Credits
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "DataFiller.h"
#include "AliReducedTrackInfo.h"
#include "AliReducedVarManager.h"

void Qn::DataFiller::FillDetectors(Qn::DataFiller::MapDetectors &channel,
                                   Qn::DataFiller::MapDetectors &tracking,
                                   const VarManPtr &var_manager) {
  for (auto &dp : channel) { dp.second->FillData(); }
  AliReducedTrackInfo *track = nullptr;
  auto trackList = event_->GetTracks();
  TIter next(trackList);
  next.Reset();
  while ((track = (AliReducedTrackInfo *) next())!=nullptr) {
    VAR::FillTrackInfo(track, var_manager->GetVariableContainer());
    for (auto &dp : tracking) { dp.second->FillData(); }
  }
}

void Qn::DataFiller::FillEventInfo(const VarManPtr &var_manager) {
  VAR::FillEventInfo(event_, var_manager->GetVariableContainer());
}
