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

	const userPopup: PopupSettings = {
		event: 'click',
		target: 'userPopup',
		placement: 'bottom'
	};

</script>

<Modal />

{#if session}
	<nav class='grid grid-flow-row grid-cols-2 justify-between h-min w-auto mb-6 mx-6 my-4'>
		<div class='text-2xl text-firewall-red font-kollektif'>FireWall</div>
		<div class='text-mg text-black italic font-DM ml-auto mr-0 variant-filled' use:popup={userPopup}>
			<span>Hello, user!</span>
		</div>
		<div class="card p-4 bg-gray" data-popup="userPopup">
			<button on:click={logout}>Logout</button>
			<div class="variant-filled-gray" />
		</div>
	</nav>
{/if}
<slot />
