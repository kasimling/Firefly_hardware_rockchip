/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_DRM_H_
#define ANDROID_DRM_H_

#include "drmcompositor.h"
#include "drmconnector.h"
#include "drmcrtc.h"
#include "drmencoder.h"
#include "drmeventlistener.h"
#include "drmplane.h"

#include <stdint.h>
#if RK_RGA
#include <RockchipRga.h>
#endif

namespace android {
#define type_name_define(res) \
const char * res##_str(int type);

typedef struct tagPlaneGroup{
        bool     bUse;
        uint32_t zpos;
        uint32_t possible_crtcs;
        uint64_t share_id;
        std::vector<DrmPlane*> planes;
}PlaneGroup;

class DrmResources {
 public:
  DrmResources();
  ~DrmResources();

  int Init();

  int fd() const {
    return fd_.get();
  }

  const gralloc_module_t * getGralloc() {
        return gralloc_;
  }

  void setGralloc(const gralloc_module_t *gralloc) {
        gralloc_ = gralloc;
  }

  const std::vector<std::unique_ptr<DrmConnector>> &connectors() const {
    return connectors_;
  }

  const std::vector<std::unique_ptr<DrmPlane>> &planes() const {
    return planes_;
  }

   const std::vector<DrmPlane*> &sort_planes() const {
    return sort_planes_;
  }

  void DisplayChanged(void);
  void SetPrimaryDisplay(DrmConnector *c);
  void SetExtendDisplay(DrmConnector *c);

  DrmCrtc *GetCrtcFromConnector(DrmConnector *conn) const;
  DrmConnector *GetConnectorFromType(int display_type) const;
  DrmPlane *GetPlane(uint32_t id) const;
  DrmCompositor *compositor();
  DrmEventListener *event_listener();

  int GetPlaneProperty(const DrmPlane &plane, const char *prop_name,
                       DrmProperty *property);
  int GetCrtcProperty(const DrmCrtc &crtc, const char *prop_name,
                      DrmProperty *property);
  int GetConnectorProperty(const DrmConnector &connector, const char *prop_name,
                           DrmProperty *property);

  uint32_t next_mode_id();
  int SetDisplayActiveMode(int display, const DrmMode &mode);
  int SetDpmsMode(int display, uint64_t mode);
  int UpdateDisplayRoute(void);
  void ClearDisplay(void);

  int CreatePropertyBlob(void *data, size_t length, uint32_t *blob_id);
  int DestroyPropertyBlob(uint32_t blob_id);
  type_name_define(encoder_type);
  type_name_define(connector_status);
  type_name_define(connector_type);
  int DumpPlaneProperty(const DrmPlane &plane, std::ostringstream *out);
  int DumpCrtcProperty(const DrmCrtc &crtc, std::ostringstream *out);
  int DumpConnectorProperty(const DrmConnector &connector, std::ostringstream *out);
  void dump_mode(drmModeModeInfo *mode,std::ostringstream *out);

  std::vector<PlaneGroup *>& GetPlaneGroups();

#if RK_RGA
  bool isSupportRkRga() {
	RockchipRga& rkRga(RockchipRga::get());
	return rkRga.RkRgaIsReady();
  }
#endif

 private:
  int TryEncoderForDisplay(int display, DrmEncoder *enc);
  int GetProperty(uint32_t obj_id, uint32_t obj_type, const char *prop_name,
                  DrmProperty *property);

  void dump_blob(uint32_t blob_id, std::ostringstream *out);
  void dump_prop(drmModePropertyPtr prop,
                     uint32_t prop_id, uint64_t value, std::ostringstream *out);
  int DumpProperty(uint32_t obj_id, uint32_t obj_type, std::ostringstream *out);

  UniqueFd fd_;
  uint32_t mode_id_ = 0;
  bool enable_changed_;
  DrmConnector *primary_;
  DrmConnector *extend_;

  std::vector<std::unique_ptr<DrmConnector>> connectors_;
  std::vector<std::unique_ptr<DrmEncoder>> encoders_;
  std::vector<std::unique_ptr<DrmCrtc>> crtcs_;
  std::vector<std::unique_ptr<DrmPlane>> planes_;
  std::vector<DrmPlane*> sort_planes_;
  std::vector<PlaneGroup *> plane_groups_;
  DrmCompositor compositor_;
  DrmEventListener event_listener_;
  const gralloc_module_t *gralloc_;
};
}

#endif  // ANDROID_DRM_H_
