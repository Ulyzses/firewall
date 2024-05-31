<script lang='ts'>
	// COMPONENT FROM 
  import type { SvelteComponent } from 'svelte';
  import { getModalStore } from '@skeletonlabs/skeleton';

  export let parent: SvelteComponent;

  const modalStore = getModalStore();

	const formData = {
		name: '',
		macList: ['','','','','','']
	};

	function onFormSubmit(): void {
		if ($modalStore[0].response) $modalStore[0].response(formData);
		modalStore.close();
	}

	const cBase = 'card p-4 w-modal shadow-xl space-y-4 bg-white font-DM';
	const cHeader = 'text-2xl font-bold';
	const cInput = 'p-1 rounded-lg border border-gray-500';
	const cButton = 'p-2 rounded-lg border border-white'

	const numbers = [0, 1, 2, 3, 4, 5];
</script>

{#if $modalStore[0]}
	<div class="modal-example-form {cBase}">
		<header class={cHeader}>Add New Device</header>
		<article>Please choose your device's name and input your device's MAC address below.</article>
		<!-- Enable for debugging: -->
		<form class="modal-form space-y-4">
			<label class="label">
				<span>Device Name</span>
				<input class="input {cInput}" type="text" bind:value={formData.name} placeholder="Device Name" />
			</label>
			<label class="label">
				<span>Device MAC Address</span>
        <div class='flex flex-row space-x-1'>
					{#each numbers as number (number)}
						<input class="input {cInput}" type="text" minlength="2" maxlength="2" bind:value={formData.macList[number]} placeholder="00" />
						{#if number < 5}
							<span>:</span>
						{/if}
					{/each}
        </div>
			</label>
		</form>
		<!-- prettier-ignore -->
		<footer class="modal-footer {parent.regionFooter}">
			<button class="btn {parent.buttonNeutral} p-2 rounded-xl border border-black bg-black text-white hover:bg-white hover:text-black w-24" on:click={parent.onClose}>{parent.buttonTextCancel}</button>
			<button class="btn {parent.buttonPositive} p-2 rounded-xl border border-firewall-red bg-firewall-red text-white hover:bg-white hover:text-firewall-red w-24" on:click={onFormSubmit}>Submit</button>
		</footer>
	</div>
{/if}