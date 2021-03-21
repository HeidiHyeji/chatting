# Chatting
## 리눅스 기반의 터미널 채팅 프로그램🌱

### 1. usage guide
실행 터미널을 4개 준비한다. 실행 터미널 1에서 채팅 서버를 실행하고 나머지 3개의 터미널에서 채팅 클라이언트를 실행한다.

|실행 터미널-1/채팅 서버측 실행|실행터미널-2,3,4/ 채팅 클라이언트측 실행|
|------|---|
|$sudo ./cser 6553|$./ccli 127.0.0.1 6553 kim|
||$./ccli 127.0.0.2 6553 lee|
||$./ccli 127.0.0.3 6553 han|

- 채팅 서버를 포트 6553으로 실행하고, 3개의 실행 터미널을 준비해서 채팅 클라이언트를 실행한다. 각각의 채팅 클라이언트틑 대화명 kim, lee, han으로 채팅 서버에 연결한다.
- 연결 후, “Welcome to chatting room" 메세지가 뜨면, 다른 클라이언트에게 보낼 메세지를 입력한다. 
- 채팅에 참여하고 있는 클라이언트 목록을 보고 싶으면 /list 를 입력한다.
- 1:1 비밀 메세지를 보내고 싶으면 /smsg [대화명] [메세지] 형식으로 입력한다. 
- 채팅을 종료하고 싶으면 /quit 를 입력한다. 

### 2. design architecture
- 서버와 클라이언트 연결: 서버가 listening소켓, connected소켓 감시. FD_SET을 이용해서 connected 소켓 추가. 

![noname01](https://user-images.githubusercontent.com/22677083/111896663-98cc0b00-8a5e-11eb-800e-1b71b7957d24.png)
![noname02](https://user-images.githubusercontent.com/22677083/111896669-9e295580-8a5e-11eb-82ee-38589b91f8ec.png)

- list_c[] 구조체: 소켓번호, 닉네임, ip주소, 포트번호 저장.

![sfsfs](https://user-images.githubusercontent.com/22677083/111896671-a41f3680-8a5e-11eb-8ca8-eb60a3c32797.png)

- 메세지 주고 받는 원리.

![제목 sgs없음](https://user-images.githubusercontent.com/22677083/111896673-a7b2bd80-8a5e-11eb-96c1-9af7c632740d.png)

### 3. snapshots of executions
1) 연결

![noname05](https://user-images.githubusercontent.com/22677083/111896679-af726200-8a5e-11eb-8ed1-9de127f2abc0.png)

2) 메세지 보내기

![noname06](https://user-images.githubusercontent.com/22677083/111896683-b6997000-8a5e-11eb-9c29-5cbef16788a6.png)

3) 1:1 메세지

![noname07](https://user-images.githubusercontent.com/22677083/111896685-bac58d80-8a5e-11eb-8164-327a236a39d2.png)

4) 클라이언트 목록 띄우기

![noname08](https://user-images.githubusercontent.com/22677083/111896689-bdc07e00-8a5e-11eb-93ab-c160a25c8f27.png)

5)채팅 종료

![noname09](https://user-images.githubusercontent.com/22677083/111896691-c0bb6e80-8a5e-11eb-8963-8f54aecd74bc.png)


### 4. compile method
```c
  gcc -o cser cser.c 
  
  gcc -o ccli ccli.c
```
