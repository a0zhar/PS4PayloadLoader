# PS4 "PAYLOAD LAUNCHER" SOURCE CODE

### Translated Building Instructions
<details>
<summary>English</summary>

### Building Instructions
  1. First, ensure you have `gcc`, `yasm`, and `python3` installed.
  2. Once you have these dependencies installed, follow these steps:
     - Build the jbc_loader library using this command: `cd libJBC && make && cd ../`
     - Next, run `make` in the main directory, to build the rest and to create js file.
  3. After executing these two commands **in the specified order**, you will find a file named `loader.js` in the root directory. Simply include this JavaScript file in the Payload Loader ROP chain file on your PS4 web host. `loader.js` is responsible for loading the payload into memory and displaying the "launching payload" message.
</details>
<details>
<summary>Arabic (العربية)</summary>

### تعليمات البناء
  1. أولاً، تأكد من تثبيت `gcc` و `yasm` و `python3`.
  2. بمجرد أن تكون هذه التبعيات مثبتة، اتبع هذه الخطوات:
     - قم ببناء مكتبة jbc_loader باستخدام هذا الأمر: `cd libJBC && make && cd ../`
     - بعد ذلك، قم بتشغيل `make` في الدليل الرئيسي لبناء البقية وإنشاء ملف js.
  3. بعد تنفيذ هاتين الأوامر **بالترتيب المحدد**, ستجد ملفًا يُسمى `loader.js` في الدليل الرئيسي. ببساطة، قم بتضمين هذا الملف الجافا سكريبت في ملف سلسلة ROP لتحميل الحمولة في الذاكرة على مضيف الويب لجهاز PS4 الخاص بك. الملف `loader.js` مسؤول عن تحميل الحمولة في الذاكرة وعرض رسالة "جاري تشغيل الحمولة".
</details>

<details>
<summary>Chinese (中文)</summary>

### 构建说明
  1. 首先，请确保已安装 `gcc`、`yasm` 和 `python3`。
  2. 安装这些依赖项后，请按照以下步骤操作：
     - 使用以下命令构建 jbc_loader 库：`cd libJBC && make && cd ../`
     - 接下来，在主目录中运行 `make`，以构建其余部分并创建 js 文件。
  3. 在按照指定顺序执行这两个命令后，您将在根目录中找到一个名为 `loader.js` 的文件。只需在您的 PS4 web 主机上的 Payload Loader ROP 链文件中包含此 JavaScript 文件即可。`loader.js` 负责将负载加载到内存中并显示 "正在启动负载" 消息。
</details>
<details>
<summary>Russian (русский)</summary>

### Инструкции по сборке
  1. В первую очередь убедитесь, что у вас установлены `gcc`, `yasm` и `python3`.
  2. После установки этих зависимостей следуйте этим шагам:
     - Соберите библиотеку jbc_loader с помощью следующей команды: `cd libJBC && make && cd ../`
     - Затем выполните `make` в основной директории, чтобы собрать остальное и создать файл js.
  3. После выполнения этих двух команд **в указанном порядке**, вы найдете файл с именем `loader.js` в корневом каталоге. Просто включите этот файл JavaScript в файл цепочки ROP загрузчика Payload на вашем веб-хостинге PS4. Файл `loader.js` отвечает за загрузку полезной нагрузки в память и отображение сообщения "Запуск полезной нагрузки".
</details>
<details>
<summary>Korean (한국어)</summary>

### 빌드 지침
  1. 먼저 `gcc`, `yasm`, `python3`이 설치되어 있는지 확인하십시오.
  2. 이러한 종속성이 설치된 경우 다음 단계를 따르십시오:
     - 다음 명령을 사용하여 jbc_loader 라이브러리를 빌드하십시오: `cd libJBC && make && cd ../`
     - 그런 다음 주 디렉토리에서 `make`를 실행하여 나머지를 빌드하고 js 파일을 만듭니다.
  3. 이러한 두 명령을 **지정된 순서대로** 실행한 후 루트 디렉토리에 `loader.js`라는 파일이 나타납니다. 이 JavaScript 파일을 PS4 웹 호스트의 Payload Loader ROP 체인 파일에 포함하기만 하면 됩니다. `loader.js` 파일은 메모리로 페이로드를 로드하고 "페이로드 시작 중" 메시지를 표시하는 역할을 합니다.
</details>

## Explaination of Loader.js
  - The [netcat.c](https://github.com/sleirsgoevy/ps4jb2/blob/16f2e8878d462ba0abc78c04e4055444dcf8aa4a/src/netcat.c) for PS4 6.72 will load 65536 bytes at the address stored in the JS variable `mira_blob` into RWX memory and jump to it. At this point, only the minimal patches (amd64_syscall, mmap, mprotect, kexec) are applied, i.e., the process is still 'sandboxed.' After building this repo, the `mira_blob` variable inside of `loader.js` will contain the compiled **payload.bin** (miraloader i guess) data in the form of a byte array.
## Credits:
- <a href="https://github.com/sleirsgoevy">Sleirsgoevy</a> For the original files...<br>
- <a href="https://github.com/a0zhar">Me</a> For maintaining/updating the files of this repository.
