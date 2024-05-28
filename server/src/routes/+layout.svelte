<script>
	import "../app.css";
	import { goto, invalidate } from '$app/navigation';
	import { page } from '$app/stores';
	import { onMount } from 'svelte';

	export let data;
	$: ({ session, supabase } = data);

	console.log('path', $page.url.pathname);

	onMount(() => {
		const { data } = supabase.auth.onAuthStateChange((_, newSession) => {
			if (!newSession) {
				// Queue this as a task so the navigation won't prevent the triggering
				// function from completing
				setTimeout(() => {
					goto('/auth', { invalidateAll: true });
				});
			}
			if (newSession?.expires_at !== session?.expires_at) {
				invalidate('supabase:auth');
			}
		});

		return () => data.subscription.unsubscribe();
	});

	const logout = async () => {
		console.log('logging out');
		const { error } = await supabase.auth.signOut();

		if (error) {
			console.error('Sign out error', error.message);
		}

		goto('/auth');
	};
</script>

{#if session}
	<button on:click={logout}>Logout</button>
{/if}
<slot />
