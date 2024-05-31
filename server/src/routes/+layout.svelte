<script lang='ts'>
	import "../app.css";
	import { goto, invalidate } from '$app/navigation';
	import { page } from '$app/stores';
	import { onMount } from 'svelte';
	import { initializeStores, Modal } from '@skeletonlabs/skeleton';
	import { computePosition, autoUpdate, offset, shift, flip, arrow } from '@floating-ui/dom';
	import { popup } from '@skeletonlabs/skeleton';
	import type { PopupSettings } from '@skeletonlabs/skeleton';
	import { storePopup } from '@skeletonlabs/skeleton';
	storePopup.set({ computePosition, autoUpdate, offset, shift, flip, arrow });
			

	initializeStores();

	export let data;
	$: ({ session, supabase, user } = data);

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

	const userPopup: PopupSettings = {
		event: 'click',
		target: 'userPopup',
		placement: 'bottom'
	};

</script>

<Modal />

{#if session && user}
	<nav class='flex flex-row justify-between items-center h-min w-auto mb-6 mx-6 my-4'>
		<div class='text-lg sm:text-2xl text-firewall-red font-kollektif'>FireWall</div>
		<div class='flex flex-col items-center justify-end space-y-2 sm:flex-row text-xs sm:text-base text-black italic font-DM ml-auto mr-0 space-x-2'>
			<span class='flex-none'>
				Hello, 
				<span class='text-firewall-red'>{ user?.email }</span>
				!
			</span>
			<button class='self-end bg-black hover:bg-gray-200 text-white hover:text-black w-20 sm:w-24 p-1 sm:p-2 rounded-lg' on:click={logout}>Logout</button>
		</div>
	</nav>
{/if}
<slot />
