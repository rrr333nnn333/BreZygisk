import { exec, toast } from './kernelsu.js'

import { getTranslations } from './language.js'

const monitor_start = document.getElementById('monitor_start_button')
const monitor_stop = document.getElementById('monitor_stop_button')
const monitor_pause = document.getElementById('monitor_pause_button')

const monitor_status = document.getElementById('monitor_status');

(async () => {
  const sys_lang = localStorage.getItem('/ReZygisk/language')
  const translations = await getTranslations(sys_lang || 'en_US')

  if (monitor_start) {
    monitor_start.addEventListener('click', () => {
      if (![ translations.page.actions.status.tracing, translations.page.actions.status.stopping, translations.page.actions.status.stopped ].includes(monitor_status.innerHTML)) return;

      monitor_status.innerHTML = translations.page.actions.status.tracing

      exec('/data/adb/modules/brezygisk/bin/zygisk-ptrace64 ctl start')
    })

    monitor_stop.addEventListener('click', () => {
      monitor_status.innerHTML = translations.page.actions.status.exiting

      exec('/data/adb/modules/brezygisk/bin/zygisk-ptrace64 ctl exit')
    })

    monitor_pause.addEventListener('click', () => {
      if (![ translations.page.actions.status.tracing, translations.page.actions.status.stopping, translations.page.actions.status.stopped ].includes(monitor_status.innerHTML)) return;

      monitor_status.innerHTML = translations.page.actions.status.stopped

      exec('/data/adb/modules/brezygisk/bin/zygisk-ptrace64 ctl stop')
    })
  }
})()