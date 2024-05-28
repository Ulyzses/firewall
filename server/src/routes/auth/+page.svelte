<form method="POST" action="?/login">
	<label>
		Email
		<input name="email" type="email" />
	</label>
	<label>
		Password
		<input name="password" type="password" />
	</label>
	<button>Login</button>
	<button formaction="?/oauth">Sign in with Google</button>
	<a href="/auth/register">Register a new account</a>
</form>
