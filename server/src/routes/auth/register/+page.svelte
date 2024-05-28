<form method="POST" action="?/signup">
	<label>
		Email
		<input name="email" type="email" />
	</label>
	<label>
		Username
		<input name="username" type="text" />
	</label>
	<label>
		Password
		<input name="password" type="password" />
	</label>
	<button>Register</button>
	<button formaction="?/oauth">Sign in with Google</button>
</form>
