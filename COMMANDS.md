# esp_ota — Comandos úteis

Substitua `<github-user>` pelo seu usuário do GitHub.

> **Depende de:** `esp_at` — deve estar disponível como submodule ou via registry.

---

## Primeiro push (repo recém-criado no GitHub com LICENSE automática)

```bash
git remote add origin https://github.com/<github-user>/esp-ota.git
git pull origin main --allow-unrelated-histories
git checkout --ours LICENSE
git add LICENSE
git commit -m "merge: keep local LICENSE"
git push -u origin main
git push origin v0.1.0
```

---

## Fluxo de atualização (nova versão)

```bash
# 1. Faça as alterações nos arquivos

# 2. Commit
git add .
git commit -m "feat: descrição da mudança"

# 3. Nova tag de versão
git tag v0.2.0

# 4. Push
git push origin main
git push origin v0.2.0
```

---

## Usar como submodule em outro projeto

```bash
# esp_ota requer esp_at
git submodule add https://github.com/<github-user>/esp-at.git  components/esp_at
git submodule add https://github.com/<github-user>/esp-ota.git components/esp_ota
git submodule update --init
```

---

## Instalar via IDF Component Manager

```bash
idf.py add-dependency "<github-user>/esp_ota>=0.1.0"
idf.py add-dependency "<github-user>/esp_at>=0.1.0"
```

## Flash e monitorar a aplicação caso idf.py não esteja no PATH

```bash
cmd /c "call C:\esp\release-v6.0\esp-idf\export.bat && idf.py -p COM6 flash monitor"
```