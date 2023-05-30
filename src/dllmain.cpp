#include "stdafx.h"
#include "helper.hpp"

using namespace std;

HMODULE baseModule = GetModuleHandle(NULL);

inipp::Ini<char> ini;

// INI Variables
bool bAspectFix;
bool bFOVFix;
bool bUncapFPS;
bool bCutsceneFPS;
bool bDisableDOF;
bool bDisableMotionBlur;
bool bCustomRes;
int iCustomResX;
int iCustomResY;
int iInjectionDelay;
float fAdditionalFOV;
int iAspectFix;
int iFOVFix;

// Variables
float fNewX;
float fNewY;
float fNativeAspect = (float)16/9;
float fPi = 3.14159265358979323846f;
float fNewAspect;
string sFixVer = "1.0.3";

// ApplyResolution Hook
DWORD64 ApplyResolutionReturnJMP;
void __declspec(naked) ApplyResolution_CC()
{
    __asm
    {
        mov[rsp + 0x08], rbx                        // Original Code
        mov[rsp + 0x10], rsi                        // Original Code
        mov[rsp + 0x18], rdi                        // Original Code

        mov ecx, iCustomResX
        mov edx, iCustomResY
        jmp[ApplyResolutionReturnJMP]
    }
}

// Aspect Ratio/FOV Hook
DWORD64 AspectFOVFixReturnJMP;
float FOVPiDiv;
float FOVDivPi;
float FOVFinalValue;
void __declspec(naked) AspectFOVFix_CC()
{
    __asm
    {
        mov eax, [fNewAspect]                       // Move new aspect to eax
        cmp eax, [fNativeAspect]                    // Compare new aspect to native
        jle originalCode                            // Skip FOV fix if fNewAspect<=fNativeAspect
        cmp [iFOVFix], 1                            // Check if FOVFix is enabled
        je modifyFOV                                // jmp to FOV fix
        jmp originalCode                            // jmp to originalCode

        modifyFOV:
            fld dword ptr[rbx + 0x1F8]              // Push original FOV to FPU register st(0)
            fmul[FOVPiDiv]                          // Multiply st(0) by Pi/360
            fptan                                   // Get partial tangent. Store result in st(1). Store 1.0 in st(0)
            fxch st(1)                              // Swap st(1) to st(0)
            fdiv[fNativeAspect]                     // Divide st(0) by 1.778~
            fmul[fNewAspect]                        // Multiply st(0) by new aspect ratio
            fxch st(1)                              // Swap st(1) to st(0)
            fpatan                                  // Get partial arc tangent from st(0), st(1)
            fmul[FOVDivPi]                          // Multiply st(0) by 360/Pi
            fadd[fAdditionalFOV]                    // Add additional FOV
            fstp[FOVFinalValue]                     // Store st(0) 
            movss xmm0, [FOVFinalValue]             // Copy final FOV value to xmm0
            jmp originalCode

        originalCode:
            movss[rdi + 0x18], xmm0                 // Original code
            cmp [iAspectFix], 1
            je modifyAspect
            mov eax, [rbx + 0x00000208]             // Original code
            mov[rdi + 0x2C], eax                    // Original code
            jmp [AspectFOVFixReturnJMP]

        modifyAspect:
            mov eax, [fNewAspect]
            mov[rdi + 0x2C], eax                    // Original code
            jmp[AspectFOVFixReturnJMP]                         
    }
}

// UncapFPS Hook
DWORD64 UncapFPSReturnJMP;
void __declspec(naked) UncapFPS_CC()
{
    __asm
    {
        mov byte ptr[rbx+0x7A4], 0                  // bSmoothFrameRate = false
        jmp originalCode

        originalCode:
            movss xmm0, [rbx + 0x000007B8]          // Original code
            minss xmm0, xmm6                        // Original code
            movaps xmm6, xmm0                       // Original code
            jmp[UncapFPSReturnJMP]
    }
}

// FOV Culling Hook
DWORD64 FOVCullingReturnJMP;
float fOne = (float)1;
void __declspec(naked) FOVCulling_CC()
{
    __asm
    {
        movss xmm1, [fOne]                          //  r.StaticMeshLODDistanceScale | 1 = default, higher is worse
        jmp originalCode

        originalCode:
            movss[rdx + 0x000002E8], xmm1           // Original code
            movsd xmm0, [rbp + 0x000000E0]          // Original code
            jmp[FOVCullingReturnJMP]
    }
}

// Movie Interpolation Hook
DWORD64 MovieInterpReturnJMP;
void __declspec(naked) MovieInterp_CC()
{
    __asm
    {
        mov byte ptr[rcx + 0x10], 01                // Force EMovieSceneEvaluationType to be "WithSubFrames" (1) instead of "FrameLocked" (0)
        jmp originalCode

        originalCode:
            mov rbp, rsp                            // Original code
            sub rsp, 128                            // Original code
            cmp byte ptr[rcx + 0x10], 00            // Original code
            jmp[MovieInterpReturnJMP]
    }
}

// HUD Markers Hook
DWORD64 HUDMarkersReturnJMP;
float HUDXOffset = (float)0;;
float HUDYOffset = (float)0;;
void __declspec(naked) HUDMarkers_CC()
{
    __asm
    {
        cvtdq2ps xmm0, xmm0  // Original code
        movss xmm0, [HUDXOffset]
        movd xmm1, eax  // Original code
        cvtdq2ps xmm1, xmm1  // Original code
        movss xmm1, [HUDYOffset]
        subss xmm3, xmm0  // Original code

        jmp[HUDMarkersReturnJMP]
    }
}

void Logging()
{
    loguru::add_file("StarTrekResurgenceFix.log", loguru::Truncate, loguru::Verbosity_MAX);
    loguru::set_thread_name("Main");

    LOG_F(INFO, "StarTrekResurgenceFix v%s loaded", sFixVer.c_str());
}

void ReadConfig()
{
    // Initialize config
    // UE4 games use launchers so config path is relative to launcher

    std::ifstream iniFile(".\\StarTrekGame\\Binaries\\Win64\\StarTrekResurgenceFix.ini");
    if (!iniFile)
    {
        LOG_F(ERROR, "Failed to load config file.");
        LOG_F(ERROR, "Trying alternate config path.");
        std::ifstream iniFile("StarTrekResurgenceFix.ini");
        if (!iniFile)
        {
            LOG_F(ERROR, "Failed to load config file. (Alternate path)");
            LOG_F(ERROR, "Please ensure that the ini configuration file is in the correct place.");
        }
        else
        {
            ini.parse(iniFile);
            LOG_F(INFO, "Successfuly loaded config file. (Alternate path)");
        }
    }
    else
    {
        ini.parse(iniFile);
        LOG_F(INFO, "Successfuly loaded config file.");
    }

    inipp::get_value(ini.sections["StarTrekResurgenceFix Parameters"], "InjectionDelay", iInjectionDelay);
    inipp::get_value(ini.sections["Custom Resolution"], "Enabled", bCustomRes);
    inipp::get_value(ini.sections["Custom Resolution"], "Width", iCustomResX);
    inipp::get_value(ini.sections["Custom Resolution"], "Height", iCustomResY);
    inipp::get_value(ini.sections["Fix Aspect Ratio"], "Enabled", bAspectFix);
    iAspectFix = (int)bAspectFix;
    inipp::get_value(ini.sections["Fix FOV"], "Enabled", bFOVFix);
    iFOVFix = (int)bFOVFix;
    inipp::get_value(ini.sections["Fix FOV"], "AdditionalFOV", fAdditionalFOV);
    inipp::get_value(ini.sections["Uncap FPS"], "Enabled", bUncapFPS);
    inipp::get_value(ini.sections["Uncap Cutscene FPS"], "Enabled", bCutsceneFPS);
    inipp::get_value(ini.sections["Disable Depth of Field"], "Enabled", bDisableDOF);
    inipp::get_value(ini.sections["Disable Motion Blur"], "Enabled", bDisableMotionBlur);

    // Custom resolution
    if (iCustomResX > 0 && iCustomResY > 0)
    {
        fNewX = (float)iCustomResX;
        fNewY = (float)iCustomResY;
        fNewAspect = (float)iCustomResX / (float)iCustomResY;
    }
    else
    {
        // Grab desktop resolution
        RECT desktop;
        GetWindowRect(GetDesktopWindow(), &desktop);
        fNewX = (float)desktop.right;
        fNewY = (float)desktop.bottom;
        iCustomResX = (int)desktop.right;
        iCustomResY = (int)desktop.bottom;
        fNewAspect = (float)desktop.right / (float)desktop.bottom;
    }

    // Log config parse
    LOG_F(INFO, "Config Parse: iInjectionDelay: %dms", iInjectionDelay);
    LOG_F(INFO, "Config Parse: bAspectFix: %d", bAspectFix);
    LOG_F(INFO, "Config Parse: bFOVFix: %d", bFOVFix);
    LOG_F(INFO, "Config Parse: fAdditionalFOV: %.2f", fAdditionalFOV);
    LOG_F(INFO, "Config Parse: bUncapFPS: %d", bUncapFPS);
    LOG_F(INFO, "Config Parse: bCutsceneFPS: %d", bCutsceneFPS);
    LOG_F(INFO, "Config Parse: bDisableDOF: %d", bDisableDOF);
    LOG_F(INFO, "Config Parse: bDisableMotionBlur: %d", bDisableMotionBlur);
    LOG_F(INFO, "Config Parse: bCustomRes: %d", bCustomRes);
    LOG_F(INFO, "Config Parse: iCustomResX: %d", iCustomResX);
    LOG_F(INFO, "Config Parse: iCustomResY: %d", iCustomResY);
    LOG_F(INFO, "Config Parse: fNewX: %.2f", fNewX);
    LOG_F(INFO, "Config Parse: fNewY: %.2f", fNewY);
    LOG_F(INFO, "Config Parse: fNewAspect: %.4f", fNewAspect);
}

void AspectFOVFix()
{
    if (bCustomRes)
    {
        // FSystemResolution::RequestResolutionChange
        uint8_t* ApplyResolutionScanResult = Memory::PatternScan(baseModule, "8B ?? ?? ?? E8 ?? ?? ?? ?? 83 ?? ?? 77 ?? 85 ??");
        if (ApplyResolutionScanResult)
        {
            DWORD64 ApplyResolutionAddress = Memory::GetAbsolute((uintptr_t)ApplyResolutionScanResult + 0x5);
            int ApplyResolutionHookLength = Memory::GetHookLength((char*)ApplyResolutionAddress, 13);
            ApplyResolutionReturnJMP = ApplyResolutionAddress + ApplyResolutionHookLength;
            Memory::DetourFunction64((void*)ApplyResolutionAddress, ApplyResolution_CC, ApplyResolutionHookLength);

            LOG_F(INFO, "Apply Resolution: Hook length is %d bytes", ApplyResolutionHookLength);
            LOG_F(INFO, "Apply Resolution: Hook address is 0x%" PRIxPTR, (uintptr_t)ApplyResolutionAddress);
        }
        else if (!ApplyResolutionScanResult)
        {
            LOG_F(INFO, "Apply Resolution: Pattern scan failed.");
        }
    }

    if (bAspectFix)
    {
        // UCameraComponent::GetCameraView
        uint8_t* AspectFOVFixScanResult = Memory::PatternScan(baseModule, "F3 0F ?? ?? ?? ?? ?? ?? F3 0F ?? ?? ?? 8B ?? ?? ?? ?? ?? 89 ?? ?? 0F ?? ?? ?? ?? ?? ?? 33 ?? ?? 83 ?? ??");
        if (AspectFOVFixScanResult)
        {
            FOVPiDiv = fPi / 360;
            FOVDivPi = 360 / fPi;

            DWORD64 AspectFOVFixAddress = (uintptr_t)AspectFOVFixScanResult + 0x8;
            int AspectFOVFixHookLength = Memory::GetHookLength((char*)AspectFOVFixAddress, 13);
            AspectFOVFixReturnJMP = AspectFOVFixAddress + AspectFOVFixHookLength;
            Memory::DetourFunction64((void*)AspectFOVFixAddress, AspectFOVFix_CC, AspectFOVFixHookLength);

            LOG_F(INFO, "Aspect Ratio/FOV: Hook length is %d bytes", AspectFOVFixHookLength);
            LOG_F(INFO, "Aspect Ratio/FOV: Hook address is 0x%" PRIxPTR, (uintptr_t)AspectFOVFixAddress);
        }
        else if (!AspectFOVFixScanResult)
        {
            LOG_F(INFO, "Aspect Ratio/FOV: Pattern scan failed.");
        }

        // APlayerController::ProjectWorldLocationToScreenWithDistance 
        uint8_t* HUDMarkersScanResult = Memory::PatternScan(baseModule, "0F ?? ?? 66 ?? ?? ?? 0F ?? ?? F3 0F ?? ?? F3 0F ?? ?? F3 0F ?? ?? ?? F3 0F ?? ?? ?? F3 0F ?? ?? 4C");
        if (HUDMarkersScanResult)
        {
            if (fNewAspect > fNativeAspect)
            {
                HUDXOffset = ((float)iCustomResX - ((float)iCustomResY * fNativeAspect)) / 2;
            }

            if (fNewAspect < fNativeAspect)
            {
                HUDYOffset = ((float)iCustomResY - ((float)iCustomResX / fNativeAspect)) / 2;
            }

            DWORD64 HUDMarkersAddress = (uintptr_t)HUDMarkersScanResult;
            int HUDMarkersHookLength = Memory::GetHookLength((char*)HUDMarkersAddress, 13);
            HUDMarkersReturnJMP = HUDMarkersAddress + HUDMarkersHookLength;
            Memory::DetourFunction64((void*)HUDMarkersAddress, HUDMarkers_CC, HUDMarkersHookLength);

            LOG_F(INFO, "HUD Markers: Hook length is %d bytes", HUDMarkersHookLength);
            LOG_F(INFO, "HUD Markers: Hook address is 0x%" PRIxPTR, (uintptr_t)HUDMarkersAddress);
        }
        else if (!HUDMarkersScanResult)
        {
            LOG_F(INFO, "HUD Markers: Pattern scan failed.");
        }
    }

    if (bFOVFix)
    {
        // ULocalPlayer::GetProjectionData 
        uint8_t* FOVCullingScanResult = Memory::PatternScan(baseModule, "8B ?? ?? ?? ?? ?? F2 ?? ?? ?? ?? ?? 89 ?? ?? ?? 84 ?? 75 ??");
        if (FOVCullingScanResult)
        {
                DWORD64 FOVCullingAddress = (uintptr_t)FOVCullingScanResult - 0x10;
                int FOVCullingHookLength = Memory::GetHookLength((char*)FOVCullingAddress, 13);
                FOVCullingReturnJMP = FOVCullingAddress + FOVCullingHookLength;
                Memory::DetourFunction64((void*)FOVCullingAddress, FOVCulling_CC, FOVCullingHookLength);

                LOG_F(INFO, "FOV Culling: Hook length is %d bytes", FOVCullingHookLength);
                LOG_F(INFO, "FOV Culling: Hook address is 0x%" PRIxPTR, (uintptr_t)FOVCullingAddress);
        }
        else if (!FOVCullingScanResult)
        {
            LOG_F(INFO, "FOV Culling: Pattern scan failed.");
        }
    }
}

void UncapFPS()
{
    if (bUncapFPS)
    {
        uint8_t* UncapFPSScanResult = Memory::PatternScan(baseModule, "80 ?? ?? ?? ?? ?? 02 74 ?? F3 0F ?? ?? ?? ?? ?? ?? F3 0F ?? ??");
        if (UncapFPSScanResult)
        {
            // UEngine::GetMaxTickRate
            DWORD64 UncapFPSAddress = (uintptr_t)UncapFPSScanResult + 0x9;
            int UncapFPSHookLength = Memory::GetHookLength((char*)UncapFPSAddress, 13);
            UncapFPSReturnJMP = UncapFPSAddress + UncapFPSHookLength;
            Memory::DetourFunction64((void*)UncapFPSAddress, UncapFPS_CC, UncapFPSHookLength);

            LOG_F(INFO, "Uncap FPS: Hook length is %d bytes", UncapFPSHookLength);
            LOG_F(INFO, "Uncap FPS: Hook address is 0x%" PRIxPTR, (uintptr_t)UncapFPSAddress);
        }
        else if (!UncapFPSScanResult)
        {
            LOG_F(INFO, "Uncap FPS: Pattern scan failed.");
        }
    } 

    if (bCutsceneFPS)
    {
        // UEngine::GetMaxTickRate
        uint8_t* CutsceneFPSScanResult = Memory::PatternScan(baseModule, "3B ?? ?? ?? ?? ?? 0F ?? ?? F3 0F ?? ?? ?? EB ?? 0F ?? ??");
        if (CutsceneFPSScanResult)
        {
            DWORD64 CutsceneFPSAddress = (uintptr_t)CutsceneFPSScanResult + 0x9;
            Memory::PatchBytes((uintptr_t)CutsceneFPSAddress, "\x0F\x57\xC0\x90\x90", 5);
            LOG_F(INFO, "Cutscene FPS: Patch address is 0x%" PRIxPTR, (uintptr_t)CutsceneFPSAddress);
        }
        else if (!CutsceneFPSScanResult)
        {
            LOG_F(INFO, "Cutscene FPS: Pattern scan failed.");
        }

        // FMovieScenePlaybackPosition::PlayTo 
        uint8_t* MovieInterpScanResult = Memory::PatternScan(baseModule, "41 ?? 48 ?? ?? 48 ?? ?? ?? ?? 00 00 80 ?? ?? 00 49 ?? ?? 0F ?? ?? ?? ?? 48 ?? ??");
        if (MovieInterpScanResult)
        {
            DWORD64 MovieInterpAddress = (uintptr_t)MovieInterpScanResult + 0x2;
            int MovieInterpHookLength = (int)14;
            MovieInterpReturnJMP = MovieInterpAddress + MovieInterpHookLength;
            Memory::DetourFunction64((void*)MovieInterpAddress, MovieInterp_CC, MovieInterpHookLength);

            LOG_F(INFO, "Movie Interpolation: Hook length is %d bytes", MovieInterpHookLength);
            LOG_F(INFO, "Movie Interpolation: Hook address is 0x%" PRIxPTR, (uintptr_t)MovieInterpAddress);
        }
        else if (!MovieInterpScanResult)
        {
            LOG_F(INFO, "Movie Interpolation: Pattern scan failed.");
        }
    }
}

void GraphicalTweaks()
{
    if (bDisableDOF)
    {
        // DiaphragmDOF::IsSupported 
        uint8_t* DOFScanResult = Memory::PatternScan(baseModule, "74 ?? 41 ?? ?? ?? ?? C1 ?? 08 ?? 01 75 ?? 32 ?? C3 ?? 01 C3");
        if (DOFScanResult)
        {
            // Return value for DOF being enabled is changed to 0
            DWORD64 DOFAddress = (uintptr_t)DOFScanResult + 0x12;
            Memory::PatchBytes((uintptr_t)DOFAddress, "\x00", 1);

            LOG_F(INFO, "Depth of Field: Patch address is 0x%" PRIxPTR, (uintptr_t)DOFAddress);
        }
        else if (!DOFScanResult)
        {
            LOG_F(INFO, "Depth of Field: Pattern scan failed.");
        }
    }

    if (bDisableMotionBlur)
    {
        // IsMotionBlurEnabled
        uint8_t* MotionBlurScanResult = Memory::PatternScan(baseModule, "73 ?? 80 ?? ?? 00 74 ?? 85 ?? 7E ?? 48 ?? ?? ?? ?? ?? ??");
        if (MotionBlurScanResult)
        {
            // JMP to code path where motion blur is disabled
            DWORD64 MotionBlurAddress = (uintptr_t)MotionBlurScanResult + 0xA;
            Memory::PatchBytes((uintptr_t)MotionBlurAddress, "\xEB", 1);

            LOG_F(INFO, "Motion Blur: Patch address is 0x%" PRIxPTR, (uintptr_t)MotionBlurAddress);
        }
        else if (!MotionBlurScanResult)
        {
            LOG_F(INFO, "Motion Blur: Pattern scan failed.");
        }
    }
}

DWORD __stdcall Main(void*)
{
    Logging();
    ReadConfig();
    Sleep(iInjectionDelay);
    AspectFOVFix();
    UncapFPS();
    GraphicalTweaks();
    return true; // end thread
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE mainHandle = CreateThread(NULL, 0, Main, 0, NULL, 0);

        if (mainHandle)
        {
            CloseHandle(mainHandle);
        }
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

