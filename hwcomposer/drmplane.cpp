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

#define LOG_TAG "hwc-drm-plane"

#include "drmplane.h"
#include "drmresources.h"

#include <cinttypes>
#include <errno.h>
#include <stdint.h>

#include <cutils/log.h>
#include <xf86drmMode.h>

namespace android {

DrmPlane::DrmPlane(DrmResources *drm, drmModePlanePtr p)
    : drm_(drm), id_(p->plane_id), possible_crtc_mask_(p->possible_crtcs), plane_(p) {
}

int DrmPlane::Init() {
  DrmProperty p;

  int ret = drm_->GetPlaneProperty(*this, "type", &p);
  if (ret) {
    ALOGE("Could not get plane type property");
    return ret;
  }

  uint64_t type;
  ret = p.value(&type);
  if (ret) {
    ALOGE("Failed to get plane type property value");
    return ret;
  }
  switch (type) {
    case DRM_PLANE_TYPE_OVERLAY:
    case DRM_PLANE_TYPE_PRIMARY:
    case DRM_PLANE_TYPE_CURSOR:
      type_ = (uint32_t)type;
      break;
    default:
      ALOGE("Invalid plane type %" PRIu64, type);
      return -EINVAL;
  }

  ret = drm_->GetPlaneProperty(*this, "CRTC_ID", &crtc_property_);
  if (ret) {
    ALOGE("Could not get CRTC_ID property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "FB_ID", &fb_property_);
  if (ret) {
    ALOGE("Could not get FB_ID property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "CRTC_X", &crtc_x_property_);
  if (ret) {
    ALOGE("Could not get CRTC_X property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "CRTC_Y", &crtc_y_property_);
  if (ret) {
    ALOGE("Could not get CRTC_Y property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "CRTC_W", &crtc_w_property_);
  if (ret) {
    ALOGE("Could not get CRTC_W property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "CRTC_H", &crtc_h_property_);
  if (ret) {
    ALOGE("Could not get CRTC_H property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "SRC_X", &src_x_property_);
  if (ret) {
    ALOGE("Could not get SRC_X property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "SRC_Y", &src_y_property_);
  if (ret) {
    ALOGE("Could not get SRC_Y property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "SRC_W", &src_w_property_);
  if (ret) {
    ALOGE("Could not get SRC_W property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "SRC_H", &src_h_property_);
  if (ret) {
    ALOGE("Could not get SRC_H property");
    return ret;
  }

  ret = drm_->GetPlaneProperty(*this, "rotation", &rotation_property_);
  if (ret)
    ALOGE("Could not get rotation property");

  ret = drm_->GetPlaneProperty(*this, "alpha", &alpha_property_);
  if (ret)
    ALOGI("Could not get alpha property");

  ret = drm_->GetPlaneProperty(*this, "ZPOS", &zpos_property_);
  if (ret)
    ALOGE("Could not get ZPOS property");

  ret = drm_->GetPlaneProperty(*this, "SHARE_ID", &share_id_property_);
  if (ret)
    ALOGE("Could not get SHARE_ID property");

  ret = drm_->GetPlaneProperty(*this, "FEATURE", &feature_property_);
  if (ret)
    ALOGE("Could not get FEATURE property");

    b_reserved_= false;
    b_use_ = false;
    b_yuv_ = false;


    uint64_t feature=0, rotate=0;

    feature_property_.set_feature("scale");
    feature_property_.value(&feature);
    b_scale_ = (feature ==1)?true:false;


    rotation_property().set_feature("rotate");
    rotation_property().value(&rotate);
    b_rotate_ = rotate;

  return 0;
}

uint32_t DrmPlane::id() const {
  return id_;
}

bool DrmPlane::GetCrtcSupported(const DrmCrtc &crtc) const {
  return !!((1 << crtc.pipe()) & possible_crtc_mask_);
}

uint32_t DrmPlane::type() const {
  return type_;
}

const DrmProperty &DrmPlane::crtc_property() const {
  return crtc_property_;
}

const DrmProperty &DrmPlane::fb_property() const {
  return fb_property_;
}

const DrmProperty &DrmPlane::crtc_x_property() const {
  return crtc_x_property_;
}

const DrmProperty &DrmPlane::crtc_y_property() const {
  return crtc_y_property_;
}

const DrmProperty &DrmPlane::crtc_w_property() const {
  return crtc_w_property_;
}

const DrmProperty &DrmPlane::crtc_h_property() const {
  return crtc_h_property_;
}

const DrmProperty &DrmPlane::src_x_property() const {
  return src_x_property_;
}

const DrmProperty &DrmPlane::src_y_property() const {
  return src_y_property_;
}

const DrmProperty &DrmPlane::src_w_property() const {
  return src_w_property_;
}

const DrmProperty &DrmPlane::src_h_property() const {
  return src_h_property_;
}

const DrmProperty &DrmPlane::rotation_property() const {
  return rotation_property_;
}

const DrmProperty &DrmPlane::alpha_property() const {
  return alpha_property_;
}

bool DrmPlane::get_scale(){
    return b_scale_;
}

uint64_t DrmPlane::get_rotate(){
    return b_rotate_;
}

bool DrmPlane::get_yuv(){
    return b_yuv_;
}

void DrmPlane::set_yuv(bool b_yuv)
{
    b_yuv_ = b_yuv;
}

bool DrmPlane::is_use(){
    return b_use_;
}

void DrmPlane::set_use(bool b_use)
{
    b_use_ = b_use;
}

const DrmProperty &DrmPlane::zpos_property() const {
  return zpos_property_;
}

const DrmProperty &DrmPlane::share_id_property() const {
  return share_id_property_;
}

const DrmProperty &DrmPlane::feature_property() const {
  return feature_property_;
}

bool DrmPlane::is_reserved(){
  return b_reserved_;
}

void DrmPlane::set_reserved(bool b_reserved) {
    b_reserved_ = b_reserved;
}

void DrmPlane::dump_plane(std::ostringstream *out) const {
        unsigned int j;

	    *out << plane_->plane_id << "\t"
	         << plane_->crtc_id << "\t"
	         << plane_->fb_id << "\t"
	         << plane_->crtc_x << "\t"
	         << plane_->crtc_y << "\t"
	         << plane_->x << "\t"
	         << plane_->y << "\t"
	         << plane_->gamma_size << "\t"
	         << std::hex << plane_->possible_crtcs << "\n";

		if (!plane_->count_formats)
			return;

		*out << "  formats:";
		for (j = 0; j < plane_->count_formats; j++)
		    *out << &plane_->formats[j] << " "; //printf(" %4.4s", (char *)&ovr->formats[j]);

		*out << ("\n");

		drm_->DumpPlaneProperty(*this,out);
}

}