# Persiapan GitHub private

Target: `TaufikAS0/2026-esp32s3-warehouse-led`, private. Integration branch adalah develop; implementasi berada di feature/warehouse-led-controller dan dipublikasikan melalui PR ke develop. main dicadangkan untuk release produksi setelah acceptance hardware.

## Goal

Publikasikan firmware fitur yang sudah diverifikasi sebagai PR ke develop, bukan rilis produksi.

## Allowed by policy

Ya: feature/warehouse-led-controller berasal dari bootstrap develop. main baru dibuat melalui proses rilis pertama setelah acceptance hardware; tidak ada direct push main.

## Branch flow

Bootstrap develop → feature/warehouse-led-controller → reviewed PR develop → release/v0.3.2 → main dan back-merge develop saat benar-benar siap produksi.

## Commands when publication is requested

```powershell
git status --short
git diff --check
gh repo view TaufikAS0/2026-esp32s3-warehouse-led --json isPrivate,url
git ls-remote --heads origin
# Initial creation of the integration branch with the existing minimal bootstrap only.
git push --set-upstream origin develop
git push --set-upstream origin feature/warehouse-led-controller
gh repo edit TaufikAS0/2026-esp32s3-warehouse-led --default-branch develop
gh pr create --base develop --head feature/warehouse-led-controller --title "Add ESP32-S3 warehouse box LED controller" --body-file 04_Dokumen/PR_Body.md
```

Initial develop publication establishes the minimal integration baseline; feature changes never push directly to develop. Subsequent develop changes require reviewed PR. The commands assume remote is still empty: inspect its branches and ancestry before publishing; do not overwrite or force-push. Attach the created PR to the Codex chat using the app artifact tool. Do not auto-merge.

## Risks / notes

Before publication review staged files, test results, private visibility, and absence of real tokens/passwords, binary/caches, personal paths, and copied baseline secrets. Physical testing remains open: label the PR as software implementation, not a certified warehouse release. If branch protection is unavailable, enforce review through process.
