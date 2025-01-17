#
# Copyright 2019 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

# Inherit from hardware-specific part of the product configuration
$(call inherit-product, device/pantech/ef52l/device.mk)

PRODUCT_DEVICE := ef52l
PRODUCT_NAME := aosp_ef52l
PRODUCT_BRAND := VEGA
PRODUCT_MODEL := Vega Iron
PRODUCT_MANUFACTURER := Pantech
PRODUCT_RESTRICT_VENDOR_FILES := false

TARGET_VENDOR := Pantech
TARGET_VENDOR_PRODUCT_NAME := ef52l
TARGET_VENDOR_DEVICE_NAME := ef52l

#For some makefile on first run
TARGET_DEVICE := $(PRODUCT_DEVICE)

# specify phone type - needed by 2G toggle
PRODUCT_PROPERTY_OVERRIDES += \
    gsm.current.phone-type=1

# release-keys
PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="ef52l-user 4.1.2 JZ054K IM-A870L.003 release-keys" 

BUILD_FINGERPRINT := pantech/ef52l:4.1.2/JZO54K/IM-A870L.003:user/release-keys
