console.log('handler_info 파일 로딩됨. ');

// 배열 객체로 만들어 파일과 메소드 속성을 가진 순서로 기입한다.
// file : 핸들러 모둘 파일 이름
// method : 등록한 핸들러 이름
var handler_info = [
    {file : './echo', method : 'echo'}, // echo
    {file : './echo_error', method: 'echo_error'},  // echo_error
    {file : './add', method: 'add'}, // add 
    {file : './listuser', method : 'listuser'}, // listuser
    {file : './echo_encrypted',method:'echo_encrypted'}, //echo_encrypted
];

module.exports = handler_info;