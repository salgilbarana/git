var local_login = require('./passport/local_login');
var local_signup = require('./passport/local_signup');
var facebook = require('./passport/facebook');

module.exports = function (app, passport){
    console.log('config/passport 호출됨');

    // 사용자 인증에 성공했을 때 호출
    // 사용자 정보를 이용해 세션을 만듦
    // 로그인 이후에 들어오는 요청은 deserializeUser 메소드 안에서 이 세션을 확인함
    passport.serializeUser(function(user, done){
        console.log('serializeUser() 호출됨');
        console.dir(user);

        done(null, user); // 두번째 파라미터를 그다음에 처리할 함수쪽으로 전달
    });

    // 사용자 인증 이후 사용자 요청이 있을 때마다 호출
    passport.deserializeUser(function(user, done){
        console.log('deserializeUser() 호출됨.');
        console.dir(user);

        done(null, user); // req에 user 속성을 추가됨
    });

    passport.use('local-login', local_login);
    passport.use('local-signup', local_signup);
    passport.use('facebook', facebook(app, passport));

};

