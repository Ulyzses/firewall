<script lang='ts'>
	import sinigang from '$lib/assets/sinigang-and-logo.png';
	import logindesign from '$lib/assets/login-design.png';
</script>

<div class='flex flex-cols w-screen h-screen text-DM relative z-0'>
	<div class='w-full md:w-1/3 h-full bg-firewall-red text-white overflow-hidden'>
		<img src={logindesign} alt='' class='max-w-5xl self-center'>
	</div>
	<div class='w-full md:w-2/3 h-fit bg-white self-center justify-center content-center space-y-10 px-6 py-12 rounded-sm absolute z-10 md:relative md:z-0'>
		<div class='space-y-4 flex flex-col items-center'>
			<img src={sinigang} alt='Sinigang' width=125>
			<div class='text-center text-7xl text-firewall-red font-kollektif'>
				FireWall
			</div>
		</div>
		<form method="POST" action="?/login" class='flex flex-col items-center space-y-12'>
			<div class='space-y-6 content-center flex flex-col justify-center'>
				<div class='text-center text-xl font-bold'>
					Register
				</div>
				<div class='space-y-3 flex flex-col'>
					<input name="email" type="email" class='border border-gray-500 text-gray-500 p-2 text-sm rounded-lg' placeholder="Email Address" />
					<input name="username" type="text" class='border border-gray-500 text-gray-500 p-2 text-sm rounded-lg' placeholder="Username"  />
					<input name="password" type="password" class='border border-gray-500 text-gray-500 text-sm p-2 rounded-lg' placeholder="Password" />
				</div>
				<button class='border border-white p-2 w-36 bg-firewall-red text-white rounded-lg self-center font-bold hover:border-firewall-red'>Register</button>
			</div>
			<div class='space-y-6 content-center flex flex-col justify-center'>
				<button class='bg-white hover:bg-gray-200 text-black border border-gray-200 p-2 rounded-lg' formaction="?/oauth">Sign in with Google</button>
				<div class='italic font-bold text-sm'>
					Already have an account?
					<a class='text-firewall-red hover:underline' href="/auth/">Log in.</a>
				</div>
			</div>
		</form>	
	</div>
</div>
